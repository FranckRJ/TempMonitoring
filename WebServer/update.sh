#!/usr/bin/env bash

set -e

git reset --soft
git stash --include-untracked
git pull --ff-only
touch reload.touch
