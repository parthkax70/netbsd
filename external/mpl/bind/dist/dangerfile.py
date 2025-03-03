# Copyright (C) Internet Systems Consortium, Inc. ("ISC")
#
# SPDX-License-Identifier: MPL-2.0
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0.  If a copy of the MPL was not distributed with this
# file, you can obtain one at https://mozilla.org/MPL/2.0/.
#
# See the COPYRIGHT file distributed with this work for additional
# information regarding copyright ownership.

import re

# Helper functions and variables


def added_lines(target_branch, paths):
    import subprocess

    subprocess.check_output(
        ["/usr/bin/git", "fetch", "--depth", "1", "origin", target_branch]
    )
    diff = subprocess.check_output(
        ["/usr/bin/git", "diff", "FETCH_HEAD..", "--"] + paths
    )
    added_lines = []
    for line in diff.splitlines():
        if line.startswith(b"+") and not line.startswith(b"+++"):
            added_lines.append(line)
    return added_lines


def lines_containing(lines, string):
    return [l for l in lines if bytes(string, "utf-8") in l]


changes_issue_or_mr_id_regex = re.compile(rb"\[(GL [#!]|RT #)[0-9]+\]")
relnotes_issue_or_mr_id_regex = re.compile(rb":gl:`[#!][0-9]+`")
release_notes_regex = re.compile(r"doc/(arm|notes)/notes-.*\.(rst|xml)")

modified_files = danger.git.modified_files
mr_labels = danger.gitlab.mr.labels
target_branch = danger.gitlab.mr.target_branch

###############################################################################
# COMMIT MESSAGES
###############################################################################
#
# - FAIL if any of the following is true for any commit on the MR branch:
#
#     * The subject line starts with "fixup!" or "Apply suggestion".
#
#     * The subject line contains a trailing dot.
#
#     * There is no empty line between the subject line and the log message.
#
# - WARN if any of the following is true for any commit on the MR branch:
#
#     * The length of the subject line for a non-merge commit exceeds 72
#       characters.
#
#     * There is no log message present (i.e. commit only has a subject) and
#       the subject line does not contain any of the following strings:
#       "fixup!", " CHANGES ", " release note".
#
#     * Any line of the log message is longer than 72 characters.  This rule is
#       not evaluated for:
#
#         - lines starting with four spaces, which allows long lines to be
#           included in the commit log message by prefixing them with four
#           spaces (useful for pasting compiler warnings, static analyzer
#           messages, log lines, etc.),
#
#         - lines which contain references (i.e. those starting with "[1]",
#           "[2]", etc.) which allows e.g. long URLs to be included in the
#           commit log message.

fixup_error_logged = False
for commit in danger.git.commits:
    message_lines = commit.message.splitlines()
    subject = message_lines[0]
    if not fixup_error_logged and (
        subject.startswith("fixup!") or subject.startswith("Apply suggestion")
    ):
        fail(
            "Fixup commits are still present in this merge request. "
            "Please squash them before merging."
        )
        fixup_error_logged = True
    if len(subject) > 72 and not subject.startswith("Merge branch "):
        warn(
            f"Subject line for commit {commit.sha} is too long: "
            f"```{subject}``` ({len(subject)} > 72 characters)."
        )
    if subject[-1] == ".":
        fail(f"Trailing dot found in the subject of commit {commit.sha}.")
    if len(message_lines) > 1 and message_lines[1]:
        fail(f"No empty line after subject for commit {commit.sha}.")
    if (
        len(message_lines) < 3
        and "fixup! " not in subject
        and " CHANGES " not in subject
        and " release note" not in subject
    ):
        warn(f"Please write a log message for commit {commit.sha}.")
    for line in message_lines[2:]:
        if (
            len(line) > 72
            and not line.startswith("    ")
            and not re.match(r"\[[0-9]+\]", line)
        ):
            warn(
                f"Line too long in log message for commit {commit.sha}: "
                f"```{line}``` ({len(line)} > 72 characters)."
            )

###############################################################################
# MILESTONE
###############################################################################
#
# FAIL if the merge request is not assigned to any milestone.

if not danger.gitlab.mr.milestone:
    fail("Please assign this merge request to a milestone.")

###############################################################################
# VERSION LABELS
###############################################################################
#
# FAIL if any of the following is true for the merge request:
#
# * The "Backport" label is set and the number of version labels set is
#   different than 1.  (For backports, the version label is used for indicating
#   its target branch.  This is a rather ugly attempt to address a UI
#   deficiency - the target branch for each MR is not visible on milestone
#   dashboards.)
#
# * Neither the "Backport" label nor any version label is set.  (If the merge
#   request is not a backport, version labels are used for indicating
#   backporting preferences.)

backport_label_set = "Backport" in mr_labels
version_labels = [l for l in mr_labels if l.startswith("v9.")]
if backport_label_set and len(version_labels) != 1:
    fail(
        "The *Backport* label is set for this merge request. "
        "Please also set exactly one version label (*v9.x*)."
    )
if not backport_label_set and not version_labels:
    fail(
        "If this merge request is a backport, set the *Backport* label and "
        "a single version label (*v9.x*) indicating the target branch. "
        "If not, set version labels for all targeted backport branches."
    )

###############################################################################
# OTHER LABELS
###############################################################################
#
# WARN if any of the following is true for the merge request:
#
# * The "Review" label is not set.  (It may be intentional, but rarely is.)
#
# * The "Review" label is set, but the "LGTM" label is not set.  (This aims to
#   remind developers about the need to set the latter on merge requests which
#   passed review.)

if "Review" not in mr_labels:
    warn(
        "This merge request does not have the *Review* label set. "
        "Please set it if you would like the merge request to be reviewed."
    )
elif "LGTM (Merge OK)" not in mr_labels:
    warn(
        "This merge request is currently in review. "
        "It should not be merged until it is marked with the *LGTM* label."
    )

###############################################################################
# 'CHANGES' FILE
###############################################################################
#
# FAIL if any of the following is true:
#
# * The merge request does not update the CHANGES file, but it does not have
#   the "No CHANGES" label set.  (This attempts to ensure that the author of
#   the MR did not forget about adding a CHANGES entry.)
#
# * The merge request updates the CHANGES file, but it has the "No CHANGES"
#   label set.  (This attempts to ensure that the "No CHANGES" label is used in
#   a sane way.)
#
# * The merge request adds any placeholder entries to the CHANGES file, but it
#   does not target the "main" branch.
#
# * The merge request adds a new CHANGES entry that is not a placeholder and
#   does not contain any GitLab/RT issue/MR identifiers.

changes_modified = "CHANGES" in modified_files
no_changes_label_set = "No CHANGES" in mr_labels
if not changes_modified and not no_changes_label_set:
    fail(
        "This merge request does not modify `CHANGES`. "
        "Add a `CHANGES` entry or set the *No CHANGES* label."
    )
if changes_modified and no_changes_label_set:
    fail(
        "This merge request modifies `CHANGES`. "
        "Revert `CHANGES` modifications or unset the *No Changes* label."
    )

changes_added_lines = added_lines(target_branch, ["CHANGES"])
placeholders_added = lines_containing(changes_added_lines, "[placeholder]")
identifiers_found = filter(changes_issue_or_mr_id_regex.search, changes_added_lines)
if changes_added_lines:
    if placeholders_added:
        if target_branch != "main":
            fail(
                "This MR adds at least one placeholder entry to `CHANGES`. "
                "It should be targeting the `main` branch."
            )
    elif not any(identifiers_found):
        fail("No valid issue/MR identifiers found in added `CHANGES` entries.")

###############################################################################
# RELEASE NOTES
###############################################################################
#
# - FAIL if any of the following is true:
#
#     * The merge request does not update release notes and has the "Release
#       Notes" label set.  (This attempts to point out missing release notes.)
#
#     * The merge request updates release notes but does not have the "Release
#       Notes" label set.  (This ensures that merge requests updating release
#       notes can be easily found using the "Release Notes" label.)
#
# - WARN if any of the following is true:
#
#     * This merge request does not update release notes and has the "Customer"
#       label set.  (Except for trivial changes, all merge requests which may
#       be of interest to customers should include a release note.)
#
#     * This merge request updates release notes, but no GitLab/RT issue/MR
#       identifiers are found in the lines added to the release notes by this
#       MR.

release_notes_regex = re.compile(r"doc/(arm|notes)/notes-.*\.(rst|xml)")
release_notes_changed = list(filter(release_notes_regex.match, modified_files))
release_notes_label_set = "Release Notes" in mr_labels
if not release_notes_changed:
    if release_notes_label_set:
        fail(
            "This merge request has the *Release Notes* label set. "
            "Add a release note or unset the *Release Notes* label."
        )
    elif "Customer" in mr_labels:
        warn(
            "This merge request has the *Customer* label set. "
            "Add a release note unless the changes introduced are trivial."
        )
if release_notes_changed and not release_notes_label_set:
    fail(
        "This merge request modifies release notes. "
        "Revert release note modifications or set the *Release Notes* label."
    )

if release_notes_changed:
    notes_added_lines = added_lines(target_branch, release_notes_changed)
    identifiers_found = filter(relnotes_issue_or_mr_id_regex.search, notes_added_lines)
    if notes_added_lines and not any(identifiers_found):
        warn("No valid issue/MR identifiers found in added release notes.")
else:
    notes_added_lines = []

###############################################################################
# CVE IDENTIFIERS
###############################################################################
#
# FAIL if the merge request adds a CHANGES entry of type [security] and a CVE
# identifier is missing from either the added CHANGES entry or the added
# release note.

if lines_containing(changes_added_lines, "[security]"):
    if not lines_containing(changes_added_lines, "(CVE-20"):
        fail(
            "This merge request fixes a security issue. "
            "Please add a CHANGES entry which includes a CVE identifier."
        )
    if not lines_containing(notes_added_lines, "CVE-20"):
        fail(
            "This merge request fixes a security issue. "
            "Please add a release note which includes a CVE identifier."
        )

###############################################################################
# PAIRWISE TESTING
###############################################################################
#
# FAIL if the merge request adds any new ./configure switch without an
# associated annotation used for pairwise testing.

configure_added_lines = added_lines(target_branch, ["configure.ac"])
switches_added = lines_containing(
    configure_added_lines, "AC_ARG_ENABLE"
) + lines_containing(configure_added_lines, "AC_ARG_WITH")
annotations_added = lines_containing(configure_added_lines, "# [pairwise: ")
if len(switches_added) > len(annotations_added):
    fail(
        "This merge request adds at least one new `./configure` switch that "
        "is not annotated for pairwise testing purposes."
    )

###############################################################################
# USER-VISIBLE LOG LEVELS
###############################################################################
#
# WARN if the merge request adds new user-visible log messages (INFO or above)

user_visible_log_levels = [
    "ISC_LOG_INFO",
    "ISC_LOG_NOTICE",
    "ISC_LOG_WARNING",
    "ISC_LOG_ERROR",
    "ISC_LOG_CRITICAL",
]
source_added_lines = added_lines(target_branch, ["*.[ch]"])
for log_level in user_visible_log_levels:
    if lines_containing(source_added_lines, log_level):
        warn(
            "This merge request adds new user-visible log messages with "
            "level INFO or above. Please double-check log levels and make "
            "sure none of the messages added is a leftover debug message."
        )
        break
