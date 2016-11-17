# Hydra
## A Decentralised IRC bot network

#### System requirements

### Network Level Architecture
1. Hydra shall be a network of applications across one or more hosts
1. Each individual application connected to the network shall be known as a *node*
1. Nodes shall connect to each other by means of any pre-agreed data protocol
1. Every node shall implement the baseline hydra protocol
1. The hydra protocol shall provide a mechanism to determine a *leader* node
1. Leaders are the nodes responsible for maintaining the *Active Connection* to IRC
1. Events shall be relayed from the Leader, and replies sent to the Leader to be forwarded

### Node Level Design
2. Nodes may be written in any language so long as they can implement a data protocol
2. Nodes must connect to at least one other node in the network
2. Nodes may connect to more than one node in the network
2. Nodes shall be identified by a 32-bit ID (re-)generated on every connect attempt
2. Nodes may also keep a friendly name (*tag*)
2. Nodes must keep a record of all other nodes in the network, and the closest neighbour to them
2. Nodes should endeavor to reply to pings ASAP, lest they timeout if the delay exceeds MAX_PING
2. Nodes may reject any connection attempt as they see fit, e.g. on frequent disconnects
2. Nodes must reject termtime messages from any unregistered node

### Hydra Protocol Messages
###### NB: Nodes must relay all messages if previously unseen
#### Term (Gossip)
*A leader has been established, the network is in-term. Nodes should be transmitting data and keepalives*

3. `PING term val id`
  * Heartbeat broadcast by the leader only. Any other sender must be ignored.
  * Cohort: All nodes should reply to it. Higher term takes priority.
  * Leader: If this message is received from another id with same term, trigger an election.
  * `term` (`int`): Index of the current term. If this is greater than expected, update leader.
  * `val` (`string`): Identifier for uniqueness. Nodes must not relay the same ping twice
  * `id` (`byte[4]`): NodeID of the Leader node, not modified when relayed by other nodes
3. `PONG val id count [tag]`
  * Heartbeat reply broadcast from nodes. Iff `id` is the leader.
  * Cohort: All nodes must relay these messages once (modulo `val+id`).
  * Leader: Drop any node that doesn't respond in MAX_PING.
  * `val` (`string`): Identifier for identification
  * `id` (`byte[4]`): NodeID of the origin node, not modified when relayed by other nodes
  * `count` (`int`): Hop counter. Initialized to 0, incremented by each relay
  * `tag` (`string`): Optional, friendly name for the origin node
3. `RECV idx line`
  * Data broadcast by the leader.
  * Cohort: All nodes must relay this message once.
  * Leader: Ignore.
  * `idx` (`int`): Monotonically increasing counter for record, initialised to 0 each term
  * `line` (`string`): Unaltered full IRC message of types minimally enumerated by E_RECV
3. `SEND id line`
  * Data message to leader from a node.
  * Cohort: Ignore.
  * Leader: Forward to IRC.
  * `id` (`byte[4]`): NodeID of the origin node, not modified when relayed by other nodes
  * `line` (`string`): Full IRC message of types minimally enumerated by E_SEND
3. `DROP id [reason]`
  * Broadcast to remove a node from the network.
  * All: Comply instantly.
  * `id` (`byte[4]`): NodeID of the node to be removed.
  * `reason` (`string`): Optional, reason for removal

#### Election (Paxos)
*A leader needs to be established, election messages ___only___*

4. `NOMINATE term id`
  * Nominate node `id` (self) for election.
  * Cohort: Receiving this message in termtime signifies start of an election.
  * Leader: Receiving this message terminates the term. Drop leader status, vote.
  * Nominee: Take nominee status. Ignore any other nominations.
  * `term` (`int`): Index of the term in election. Ignore if less than expected.
  * `id` (`byte[4]`): Nominee's nodeID
4. `PLEDGE term nom id`
  * Pledge to vote for node `id` in election `term`.
  * Cohort: All nodes must pledge to vote for the first nomination they see.
  * Nominee: On recieving a quorum of pledges within MAX_VOTE, call the election.
  * `term` (`int`): Index of the term in election. Ignore if less than expected.
  * `nom` (`byte[4]`): Nominee's nodeID
  * `id` (`byte[4]`): Sender's nodeID
4. `CALL term`
  * Fetch votes for election `term`.
  * Cohort: Relay if not seen. Respond with pledged vote.
  * Nominee: Reset timer, expect quorum of votes within MAX_VOTE.
  * `term` (`int`): Index of the term in election. Ignore if less than expected or no pledges.
4. `ELECT term nom id`
  * Vote in election `term`.
  * Cohort: All nodes must relay this message once.
  * Nominee: Take leader status upon quorum of votes within MAX_VOTE. Begin term with PING.
  * `term` (`int`): Index of the term in election. Ignore if less than expected.
  * `nom` (`byte[4]`): Nominee's nodeID
  * `id` (`byte[4]`): Sender's nodeID

#### Induction (2PC)
*A server has requested to be a node, collectively decide whether to induct, if in-term*

5. `KNOCK id med`
  * Induction request sent by mediator on behalf of a newly linked server
  * Cohort: If not seen recently, record this `id` and expect HELLO within MAX_MEET.
  * Mediator: Take mediator status. Ignore any other attempts to induct.
  * `id` (`byte[4]`): Proposed NodeID generated by and for the new server
  * `med` (`byte[4]`): NodeID of the mediator node
5. `MEET id med`
  * Induction response broadcast, if agreed on induction.
  * Cohort: All nodes must relay this message once.
  * Mediator: Send WELCOME if/once all nodes consent within MAX_MEET.
  * `id` (`byte[4]`): Recieved NodeID. Must be ignored if conflicting or already met in the last MAX_MEET
  * `med` (`byte[4]`): NodeID of the mediator node
5. `WELCOME id`
  * Induction notification sent by mediator to new node on reciept of all MEETs within MAX_MEET
  * Inductee: Take node status.
  * Mediator: Drop mediator status.
  * `id` (`byte[4]`): Accepted NodeID
5. `HELLO id [tag]`
  * Induction confirmation from incoming node
  * Cohort: Register node and relay if not already seen.
  * `id` (`byte[4]`): Accepted NodeID. Must be ignored unless `id` has knocked
  * `tag` (`string`): Optional, friendly name for the origin node

### Annex A - Definitions
E_RECV :
  `JOIN`, `PART`, `PRIVMSG`, `NOTICE`, `MODE`, `TOPIC`, `QUIT` or `KICK`

E_SEND :
  `JOIN`, `PART`, `PRIVMSG`, `NOTICE`, `MODE`, `TOPIC` or `KICK`

MAX_PING :
  `10` seconds
  
MAX_VOTE :
  Vary between `5`-`15` seconds
  
MAX_MEET :
  `10` seconds
  
### Annex B - Example Data Protocols
* TCP data over agreed port
* UDP data on agreed port
* IRC message (separate from the active connection)
* FIFO message-passing
