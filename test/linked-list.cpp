#include <iostream>

using std::cout;

struct List {
  List *next;
  int value;
};

List *generateList() {
  int random[] = {3, 1, 4, 1, 5, 9, 2, 6, 5, 3, 5};
  List *head = nullptr;
  for (auto value : random) {
    head = new List{head, value};
  }
  return head;
}

void freeList(List *head) {
  List *next;
  while (nullptr != head) {
    next = head->next;
    delete head;
    head = next;
  }
}

int main() {
  List *head = generateList();
  List *iterator = head;
  while (nullptr != iterator) {
    iterator->value += 1;
    iterator = iterator->next;
  }
  cout << head->value << '\n';
  freeList(head);
  return 0;
}
