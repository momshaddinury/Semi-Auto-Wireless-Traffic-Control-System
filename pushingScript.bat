@echo off
git add *
set /p commitMessage="Enter Commit Message: "
git commit -m "%commitMessage%"
git push origin
pause