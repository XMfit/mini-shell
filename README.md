# mini-shell
A mini shell created in C for educational purposes. Current features: 
- Internal shell commands
- Long term history logging
- Environment management
- External command execution via POSIX system calls
- Up arrow cycling, and tab completion via GNU Readline library.
- Advanced I/O operations via file descriptors

### Build
`make all`

This program uses the GNU Readline library, which is licensed under the GPL.
For more information, refer to the GNU Readline documentation.

### To-do 
- Improve autocompletion command list 
- Improve exportion and deleting variables from `.envi` file 
- Improve I/O redirection to work with different successive redirects
- Fix issue when running command that doesn't exist shell process is duplicated
