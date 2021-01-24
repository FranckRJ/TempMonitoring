#!/usr/bin/env bash

git reset --soft
git stash --include-untracked
git pull --ff-only
touch reload.touch
