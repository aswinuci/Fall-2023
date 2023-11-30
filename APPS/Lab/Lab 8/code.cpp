class Solution {
public:
    double maxProbability(int n, vector<vector<int>>& edges, vector<double>& succProb, int start_node, int end_node) {
        vector<vector<pair<int,double>>>graph(n);
        int i=0;
        for(auto e:edges){
            graph[e[0]].push_back({e[1],succProb[i]});
            graph[e[1]].push_back({e[0],succProb[i]});
            i++;
        }

        priority_queue<pair<double,int>>q;
        vector<bool>visited(n,false);
        q.push({1.0,start_node});
        vector<double>dist(n,0);
        dist[start_node]=1.0;
        while(!q.empty()){
            auto top = q.top();
            q.pop();
            int node = top.second;
            double proba = top.first;
            if(visited[node])continue;
            visited[node]=true;

            for(auto conn:graph[top.second]){
                    if(dist[conn.first] < conn.second*proba){
                        dist[conn.first] = conn.second*proba;
                        q.push({dist[conn.first],conn.first});
                    }
                }
        }
        return dist[end_node];
    }
};