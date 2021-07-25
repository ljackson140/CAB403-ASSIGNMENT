
# CAB403 Systems Programming - Process Overseer

This software application was developed within an virtual machine provided by QUT, will still be able to run on Mac or Linux devices without the virtual machine.

The Process Overseer primarily executes command line inputs and outputs responses within the overseer screen.

# Overview of sections
Section | Argument | Description
--------| -------|----------------
A       | <address>    | overseer IP address
A       | <port>       | overseer port
A       |  <file>      | the file to be executed  
A       | [arg...]     | an arbitrary quantity of arguments passed to file
B       | --help       | prints usage message
B       | [-o out_file]     | redirects stdout and stderr of the executed <file> to out_file
B       | [-log log_file]   | redirects stdout of the overseer’s management of <file> to log_file 
D       | [-t seconds]      | specifies the timeout for SIGTERM to be sent 
E       | mem [pid]         | get memory information  
E       | memkill <percent> | kill children if they use too much memory


## Authors

- [Laku Jackson](https://www.github.com/ljackson140)
- [Saw Soe Moe Nyunt](https://github.com/sawsoe)
- [Aung Khant Kyaw](https://github.com/aung-qut)

## Dependencies 
* Process Overseer utilizes the POSIX system.

## Notes
This application consist of 2 applications communicating through BSD Sockets.

* The Overseer program continuously runs and handles the clients (Controller) calls.
* The Controller only runs for an instant at a time; it is executed with varying arguments to issue commands to the overseer, then terminates.



## Application Setup 

Traverse to the folder directory 

```bash
Step 1.
  Unzip folder 
  input "make" on command line 

Step 2.
  input "./overseer <4 digit Pin>" on the current command line 

Step 3.
  open up another terminal for the controller
  within that terminal input "./controller localhost <4 digit Pin>"

Step 4.
  input each argument into the "controller" command line
  ex. "controller <address> <port> {[-o out_file] [-log log_file] [-t seconds]
<file> [arg...] | mem [pid] | memkill <percent>}"

Tip.
Input arguments for each section (C is only an threadpool for the overseer). Sections A, B, C are complete and functional.
```
    