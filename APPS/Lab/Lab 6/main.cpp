class Solution {
public:
    int findHeight(TreeNode* root){
        if(!root)return 0;
        if(root && root->left && root->right)return 1 + max(findHeight(root->left),findHeight(root->right));
        if(root->left)return 1 + findHeight(root->left);
        return 1+findHeight(root->right);
    }

    void assignMatrix(vector<vector<string>> &ans,TreeNode *root,int r,int c,int height){
        if(!root)return;
        ans[r][c]=to_string(root->val);
        if(root->left)assignMatrix(ans,root->left,r+1,c - pow(2,height-r-1),height);
        if(root->right)assignMatrix(ans,root->right,r+1,c+pow(2,height-r-1),height);
    }

    vector<vector<string>> printTree(TreeNode* root) {   
        int m = findHeight(root);
        int height = m-1;
        int n = pow(2,m)-1;
        vector<vector<string>>ans(m,vector<string>(n,""));
        int middle = n/2;
        assignMatrix(ans,root,0,(n-1)/2,height);
        return ans;
    }
};