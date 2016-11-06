# Hydra
## A Decentralised IRC bot network

### System requirements specification

#### Network Level Architecture
1. Hydra shall be a network of applications across one or more hosts
1. Each individual application connected to the network shall be known as a *node*
1. Nodes shall connect to each other by means of any pre-agreed data protocol
1. Every node shall implement the baseline hydra protocol
1. The hydra protocol shall provide a mechanism to determine a *leader* node
1. Leaders are the nodes responsible for maintaining the *Active Connection* to IRC
1. Events shall be relayed from the Leader, and replies sent to the Leader to be forwarded
1. Replies shall take the form of a `JOIN`, `PART`, `PRIVMSG`, `NOTICE`, `MODE` or `KICK` message

#### Node Level Design
2. Nodes may be written in any language so long as they can implement a data protocol
2. Nodes must connect to at least one other node in the network
2. Nodes may connect to more than one node in the network
2. Nodes shall be identified by a 32-bit ID (re-)generated on every connect attempt
2. Nodes may also keep a friendly name (*tag*)
2. Nodes must keep a record of all other nodes in the network, and the fastest route to them

#### Hydra Protocol Messages
3. PING val id
  * Heartbeat broadcast by the leader. All nodes must relay this message, and should reply to it
  * val (string): Identifier for uniqueness. Nodes must not relay the same ping twice
  * id (byte[4]): NodeID of the Leader node, not modified when relayed by other nodes
3. PONG val id count [tag]
  * Heartbeat reply broadcast from nodes. All nodes must relay these messages
  * val (string): Identifier for identification
  * id (byte[4]): NodeID of the origin node, not modified when relayed by other nodes
  * count (int): Hop counter. Initialized to 0, incremented by each relay
  * tag (string): Optional, friendly name for the origin node

#### Annex A - Example Data Protocols
* TCP data over agreed port
* UDP data on agreed port
* IRC message (separate from the active connection)
* FIFO message-passing
