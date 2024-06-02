# XOs Multiplayer Game
This is a novel and (slightly overengineered) approach to a clasic game - Naughts and Crosses - that will allow users to play naughts and crosses online with friends. The purpose of this project was moreso an educational one were I aimed to further my 
- C++ skills
- Learn socket programming
- Learn multithreading and how a server handles multiple connections at once
- Broaden understanding of TCP/IP
- Gain a deeper appreciation for the design and development of application layer protocols

## Features
- Fully functioning application protocol designed specfically for naughts and crosses (more info below)
- Server and client code allowing you to play with your friends (self hosting of server required)
- multithreading support on server

## XOs Protocol design
The protocol is a byte oriented protocol that contains a three byte header followed by an optional payload.
### Header
| Field  | Size | Desc |
| ------------- | ------------- | ------------- |
| Command  | 1 byte  | 
| Sender ID  | 1 byte  | id should be allocated by server on first connect, prior to <br />this the value should be 0x0. The servers id is 0xff  |
| Payload Size  | 1 byte  |

### Commands
| Command  | Value | Client Payload | Server Payload | Desc |
| ------------- | ------------- | ------------- | ------------- | ------------- |
| JOIN  | 0x00  | 15 bytes - Username | 1 byte - Allocated user ID if succesful otherwise 0 | Allows user to connect and allocates them and ID if succesful |
| LIST  | 0x01  | No payload req | n*16 bytes - with 1 byte id and 15 byte username for n users | Returns a list of online users who can be challenged |
| CHALLENGE  | 0x02 | 1 byte - id of person challenged | No payload req | Challenges another online user |
| CHALLENGERS  | 0x03 | No payload req | n*16 bytes - with 1 byte id and 15 byte username for n users | Returns a list of users who have challenged you |
| MOVE  | 0x04 | 1 byte - game id, 1 - byte move position | No payload req | Send a move for a game and validates wether it is allowed |
| GAME_STATE  | 0x05 | No payload req | 1 byte - game id, 1 byte - game state flag (more below), 9 bytes - game state | Returns the game state |
| DISCONNECT  | 0x06 | No payload req | No payload req | Disconencts the client |

### Game state flags
| Flag  | Value | Desc |
| ------------- | ------------- | ------------- |
| VALID_MOVE  | 0  | Move made is valid |
| INVALID_MOVE  | 1  | Moved made by client is invalid | 
| GAME_WON  | 2  | The client has won the game | 
| GAME_LOST  | 3  | The client has lost the game | 
| GAME_DRAWN  | 4  | The client has drawn the game | 



