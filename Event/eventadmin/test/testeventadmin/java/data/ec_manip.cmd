lm
lc
cg TestEventChannel1
cg TestEventChannel2
cg TestEventChannel3
lc
connect TestEventChannel1 TestEventChannel2
u TestEventChannel3 URL=corbaloc:iiop:amidala:21410/DefaultEventChannelFactory
cg TestEventChannel3
destroy TestEventChannel3
lc
lp
dp TestEventChannel2
create_profile toto URL=corbaloc:iiop:localhost:21410/DefaultEventChannelFactory
lp
dp toto
rm toto
lp
x
