struct ListNode {
 	int val;
 	struct ListNode *next;
 	ListNode(int x) : val(x), next(nullptr) {}
  };

  class Solution {
public:
    bool hasCycle(ListNode *head) {
        
        
        ListNode *fast,*slow;
        fast = head;slow = head;
        while(fast != nullptr && fast->next != nullptr)
        {
            //if(fast->next == nullptr || fast->next->next == nullptr) return false;
            fast = fast->next->next;
            slow = slow->next;
            if(fast == slow)
            {
                return true;
            }
        }
        return false;
    }
};