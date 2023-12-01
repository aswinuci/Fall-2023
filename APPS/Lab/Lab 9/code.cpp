#include <vector>
#include <algorithm>

class Solution {
public:
    int mincostTickets(vector<int>& days, vector<int>& costs) {
        vector<int> dp(366, 0);
        int n = days.size();
        int index = 0;

        for (int i = 1; i <= 365; ++i) {
            if (index < n && days[index] == i) {
                dp[i] = min({ dp[i - 1] + costs[0], dp[max(0, i - 7)] + costs[1], dp[max(0, i - 30)] + costs[2] });
                ++index;
            } else {
                dp[i] = dp[i - 1];
            }
        }

        return dp[days[n - 1]];
    }
};
