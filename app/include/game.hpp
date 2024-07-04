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

  auto getId() -> int { return id; }
  auto getName() -> std::string { return name; }
  auto getPath() -> std::string { return path; }
};
