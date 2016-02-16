#define NULL 0

typedef struct ListNode {
  void *payload;
  struct ListNode *next;
} ListNode;

/// Applies `f` to the payload of every element of the list that starts at
/// `head`.
void applyToList(ListNode *head, void (*f)(void *)) {
  ListNode *iterator = head;
  while (iterator != NULL) {
    f(iterator->payload);
    iterator = iterator->next;
  }
}
