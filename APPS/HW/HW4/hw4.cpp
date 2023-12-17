class Solution {
public:
    ListNode* reverseKGroup(ListNode* head, int k) {
        ListNode* current = head;
        int count = 0;

        // Count the number of nodes in the current group
        while (current != nullptr && count < k) {
            current = current->next;
            count++;
        }

        // If there are at least k nodes in the group, reverse them
        if (count == k) {
            current = reverseKGroup(current, k); // Recursively reverse the next group
            while (count > 0) {
                ListNode* next = head->next;
                head->next = current;
                current = head;
                head = next;
                count--;
            }
            head = current;
        }

        return head;
    }
};
