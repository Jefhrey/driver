savedcmd_mydev.mod := printf '%s\n'   mydev.o | awk '!x[$$0]++ { print("./"$$0) }' > mydev.mod
