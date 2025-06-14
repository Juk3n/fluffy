# fluffy - CLI Game Launcher for Linux

Fluffy's goal is to provide simple and fast possibility to run games from user defined directories and store all the required paths in one place.

## Usage

### Command-line options

- fluffy show - show list of games

```console
user:~/some/path$ fluffy show
game_name: /path/to/your/game
```

- fluffy add <game_name> <game_path> - add a game

```console
user:~/some/path$ fluffy add game_name path/to/your/game
```

- fluffy rm <game_name> - remove a game

```console
user:~/some/path$ fluffy rm game_name
```

- fluffy - runs a fluffy gui from which the game can be run
![fluffy 0.1](screenshots/fluffy_0_1.png)

## Installation

- Move fluffy to the target folder (after the first run it will create an sql file)
- (optional) Add fluffy to PATH in .bashrc file
```console
export PATH=$PATH:/path/to/target/folder/fluffy
```
- Downloaded file is ready to use

### Auto-completion (optional)

- Move fluffy-completion.bash script to target folder
- Add below line to .bashrc file
```console
source /path/to/target/folder/fluffy-completion.bash
```
