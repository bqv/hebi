# Hydra
## A Decentralised IRC bot network

### System requirements specification


#### Architecture

1. Hydra shall be a network of applications across one or more hosts
1. Each individual application shall be known as a node
1. Nodes shall connect to each other by means of any pre-agreed data protocol
1. Every node shall implement the baseline hydra protocol
1. The hydra protocol shall provide a mechanism to determine a Leader node
1. Leaders are the node responsible for connecting to IRC
1. Events shall be relayed from the Leader, and replies sent to the Leader to be forwarded
1. Replies shall take the form of a JOIN, PART, PRIVMSG, NOTICE, MODE, KICK

#### Protocol
