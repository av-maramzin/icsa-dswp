#include <iostream>

using std::cout;

struct List {
  List *next;
  int value;
};

struct LList {
  LList *next;
  List *value;
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

LList *generateLList() {
  const int R = 10;
  LList *head = nullptr;
  for (int i = 0; i < R; ++i) {
    head = new LList{head, generateList()};
  }
  return head;
}

void freeLList(LList *head) {
  LList *next;
  while (nullptr != head) {
    next = head->next;
    freeList(head->value);
    delete head;
    head = next;
  }
}

int main() {
  LList *head = generateLList();
  LList *i = head;
  int sum = 0;
  while (nullptr != i) {
    List *j = i->value;
    while (nullptr != j) {
      sum += j->value;
      j = j->next;
    }
    i = i->next;
  }
  cout << sum << '\n';
  freeLList(head);
  return 0;
}
