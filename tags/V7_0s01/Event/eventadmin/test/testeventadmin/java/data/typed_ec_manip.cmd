lm
lc
ct TestTypedEventChannel1
ct TestTypedEventChannel2
ct TestTypedEventChannel3
lc
connect TestTypedEventChannel1 TestTypedEventChannel2 IDL:test.eventadmin.cdmw/TypedEcho:1.0
u TestTypedEventChannel3 URL=corbaloc:iiop:localhost:21410/DefaultTypedEventChannelFactory IDL=IDL:test.eventadmin.cdmw/TypedEcho:1.0
ct TestTypedEventChannel3
lc
destroy TestTypedEventChannel3
lc
lp
dp TestTypedEventChannel2
create_profile toto URL=corbaloc:iiop:localhost:21410/DefaultTypedEventChannelFactory
lp
dp toto
rm toto
lp
stop_ec_manager
x
