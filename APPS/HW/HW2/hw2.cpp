class Solution {
public:
    vector<vector<int>> fourSum(vector<int>& nums, int target) {
        int n = nums.size();
        sort(nums.begin(), nums.end());
        vector<vector<int>> ans;
        for (int i = 0; i < n - 3; i++) {
            for (int j = i + 1; j < n - 2; j++) {
                long long twoSumTarget = (long long)target - (long long)nums[i] - (long long)nums[j];
                int left = j + 1, right = n - 1;
                while (left < right) {
                    if (nums[left] + nums[right] < twoSumTarget) {
                        left++;
                    } else if (nums[left] + nums[right] > twoSumTarget) {
                        right--;
                    } else {
                        ans.push_back({nums[i], nums[j], nums[left], nums[right]});
                        int tempLeft = left, tempRight = right;
                        while (left < right && nums[left] == nums[tempLeft]) left++;
                        while (left < right && nums[right] == nums[tempRight]) right--;
                    }
                }
                while (j + 1 < n && nums[j] == nums[j + 1]) j++;
            }
            while (i + 1 < n && nums[i] == nums[i + 1]) i++;
        }
        return ans;
    }
};
