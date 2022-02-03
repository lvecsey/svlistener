
*Overview*

svlistener can be run in the foreground, as a daemontools service.

It will listen to UDP packets on port 5925, and based on the service name passed in it will send a start command to it.

*Usage*

```console
make svlistener
make send-svc
```

Place the svlistener on each server or cluster node, for example as /usr/bin/svlistener. Set it up as a daemontools service with a run script.

From any other node you can then run:

```console
send-svc service_name ip_address1 [ip_address2] [ip_address3] ..
```

and it will run the appropriate ``svc -o /etc/service/service_name`` command on the given node.
