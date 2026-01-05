#include <vector>
struct ListNode {
 	int val;
 	struct ListNode *next;
 	ListNode(int x) : val(x), next(nullptr) {}
  };

  class Solution {
public:
    /**
     * 代码中的类名、方法名、参数名已经指定，请勿修改，直接返回方法规定的值即可
     *
     * 
     * @param lists ListNode类vector 
     * @return ListNode类
     */

    ListNode* mergeKLists(vector<ListNode*>& lists) {
        // write code here
        if (lists.empty()) return nullptr;
        return merge(lists, 0, lists.size() - 1);
    }
private:
    ListNode* merge(vector<ListNode*>& lists, int left, int right) {
        if (left > right) return nullptr;
        if (left == right) return lists[left];
        
        int mid = left + (right - left) / 2;
        ListNode* l1 = merge(lists, left, mid);
        ListNode* l2 = merge(lists, mid + 1, right);
        return mergeTwoLists(l1, l2);
    }
    ListNode* mergeTwoLists(ListNode* l1, ListNode* l2) {
        ListNode* dummy = new ListNode(0);  // 虚拟头节点
        ListNode* cur = dummy;
        
        while (l1 && l2) {
            if (l1->val <= l2->val) {
                cur->next = l1;
                l1 = l1->next;
            } else {
                cur->next = l2;
                l2 = l2->next;
            }
            cur = cur->next;
        }
        
        cur->next = l1 ? l1 : l2;
        
        ListNode* result = dummy->next;
        delete dummy;  // 释放虚拟头节点
        return result;
    }

};