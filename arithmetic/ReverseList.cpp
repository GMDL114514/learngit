struct ListNode {
    int val;
    struct ListNode *next;
    ListNode(int x) :
        val(x), next(nullptr) {
    }
};

class Solution {
public:
    /**
     * 代码中的类名、方法名、参数名已经指定，请勿修改，直接返回方法规定的值即可
     *
     * 
     * @param head ListNode类 
     * @param k int整型 
     * @return ListNode类
     */
    ListNode* reverseKGroup(ListNode* head, int k) {
        // write code here
        if (!head || k <= 1) return head;
    
        // 统计长度
        int len = 0;
        ListNode* cur = head;
        while (cur) {
            len++;
            cur = cur->next;
        }
    
        ListNode* dummy = new ListNode(0);
        dummy->next = head;
        ListNode* pre = dummy;
    
        for (int i = 0; i < len / k; i++) {
           // 头插法反转当前组
            ListNode* curr = pre->next;
            for (int j = 1; j < k; j++) {
                ListNode* temp = curr->next;
                curr->next = temp->next;
                temp->next = pre->next;
                pre->next = temp;
            }
            pre = curr;
        }
    
        ListNode* result = dummy->next;
        delete dummy;
        return result;
    }
};