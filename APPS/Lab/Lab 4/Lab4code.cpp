#include<bits/stdc++.h>
using namespace std;

struct ListNode {
    int val;
    ListNode* next;
    ListNode(int x): val(x), next(nullptr) {}
};

class Solution {
public:
    ListNode* addTwoNumbers(ListNode* l1, ListNode* l2) {
        stack<int> s1, s2;
        ListNode* current = l1;
        
        while (current) {
            s1.push(current->val);
            current = current->next;
        }
        
        current = l2;
        
        while (current) {
            s2.push(current->val);
            current = current->next;
        }
        
        int carry = 0;
        ListNode* result = nullptr;
        
        while (!s1.empty() || !s2.empty() || carry) {
            int sum = carry;
            
            if (!s1.empty()) {
                sum += s1.top();
                s1.pop();
            }
            
            if (!s2.empty()) {
                sum += s2.top();
                s2.pop();
            }
            
            carry = sum / 10;
            sum %= 10;
            
            ListNode* newNode = new ListNode(sum);
            newNode->next = result;
            result = newNode;
        }
        
        return result;
    }
};

int main(int argc, char const *argv[])
{
    /* code */
    return 0;
}
