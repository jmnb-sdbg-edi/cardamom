sleep 10
xterm  -sb -sl 5000 -e TimeAdmin  --CdmwFederatedCtrlClock --CdmwLocalisationService=4883 -ORBInitRef ClockService=corbaloc::localhost:4883/ClockService &

