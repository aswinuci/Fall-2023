class Solution {
public:
    int maximumElementAfterDecrementingAndRearranging(vector<int>& arr) {
        int desiredNumber=1;
        sort(arr.begin(),arr.end());
        arr[0]=1;
        for(int i=1;i<arr.size();i++){
            if(arr[i]-arr[i-1]<=1)continue;
            arr[i]=arr[i-1]+1;
        }
        return arr[arr.size()-1];
    }
};