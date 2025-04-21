pipeserver
==========

Pipeserver reads input from a pipe and feeds it as stdin to another
process. This can reduce startup time for scripts since the process
is kept alive between invocations.

For example, imagine you use `osascript` every time a key is pressed
to open a window:

```sh
osascript -e 'tell application "Firefox" to activate'
```

This works fine except that macOS notorizes the script every time
and ends up with a 150ms startup time.

Alternatively, you can pipe that script into a pipeserver:

```sh
echo 'tell application "Firefox" to activate' \
    | pipeserver --send -- osascript -i 
```

The first invocation takes 150ms like normal, but subsequent
invocations take under 10ms.
