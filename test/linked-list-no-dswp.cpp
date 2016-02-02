#define NULL 0

typedef struct ListNode {
  void *payload;
  struct ListNode *next;
} ListNode;

void applyToList(ListNode *head, ListNode *(*next)(ListNode *)) {
  ListNode *iterator = head;
  while (iterator != NULL) {
    iterator = next(iterator);
  }
}
