# CS149-Inception
Repo for SJSU CS149 group

#### Teamwork
Be sure you are a member of the Inception team of the Theta1 organization, here https://github.com/orgs/Theta1/teams/inception this provides admin controls for merging so that we can all verify eachother's work and bring it into the master

#### Branching / Merging Strategy
To reduce chance of merging catastrophe, we're going to use some form of branching
and merging strategy. Here's how we can do it:

- Each project member creates a branch named after them.
- Each project member does work on their respective branch.
- Every time someone wants to merge back to master, create a pull request from this page: https://github.com/Theta1/CS149/pulls Do after issue is complete.
- 2nd Someone will then review the pull request and merge them if satisfactory. 
- Each project member needs to rebase frequently from master to their own branch to make sure their branch 
is up-to-date.
- When a rebase fails, be sure to use $git status and $git diff to view the conflicts then use a text editor to resolve the file. Problem sections will be marked by <<<<<...VERSION1...=====...VERSION2...>>>>

#### Checkout 
- git checkout (branch) //to change branch.
- git checkout (commit) //to look at an older commit
- git checkout (commit) (file) //to look at an older verion of just that file

#### Reverting
- git revert (commit) //undoes back to that commit, as a new commit

#### Housekeeping
- touch .gitignore
- git clean (removes untracked files from working directory)
- git branch -D <branch> (dangerious, will delete your branch will and all uncomitted changes)
