class Solution {
public:
    ListNode* oddEvenList(ListNode* head) {
        if (!head || !head->next) {
            return head; // No change needed for empty or single-node lists
        }

        ListNode* oddHead = head;
        ListNode* evenHead = head->next;
        ListNode* odd = oddHead;
        ListNode* even = evenHead;

        while (even && even->next) {
            odd->next = even->next;
            odd = odd->next;
            even->next = odd->next;
            even = even->next;
        }

        odd->next = evenHead; // Connect the odd and even parts

        return oddHead;
    }
};
