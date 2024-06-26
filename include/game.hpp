#pragma once

#include <string>

class Game {
private:
  int id;
  std::string name;
  std::string path;

public:
  Game(int id, std::string name, std::string path)
      : id{id}, name{name}, path{path} {}
  ~Game() {}

  int getId() { return id; }
  std::string getName() { return name; }
  std::string getPath() { return path; }
};
