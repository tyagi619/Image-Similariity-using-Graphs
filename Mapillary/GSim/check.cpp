    #include <bits/stdc++.h>
    using namespace std;

    #define LOCAL
    #define DEBUG

    /*
             __       ____      __  __      __  _____   __      __       __ __         __         
            /  \     |    \    |  ||  |    |  ||  __ \ |  |    |  |     /  \  \      /  /                  
           /    \    |     \   |  ||  |    |  || |_/  ||  |    |  |    /    \  \    /  /                   
          /  /\  \   |  |\  \  |  ||  |    |  ||  _  / |  |____|  |   /  /\  \  \  /  /                   
         /  /__\  \  |  | \  \ |  ||  |    |  || | \ \ |   ____   |  /  /__\  \  \/  /                      
        /  ______  \ |  |  \  \|  | \  \__/  / | |_/  ||  |    |  | /  ______  \    /                       
       /__/      \__\|__|   \_____|  \______/  |_____/ |__|    |__|/__/      \__\__/
                                                         
    */
/******************************************************************************************/
    typedef long long ll;
    typedef long double ld;
    typedef vector<ll> vll;
    typedef vector<int> vi;
    typedef pair<ll,ll> pll;
    typedef pair<int,int> ii;
    typedef vector<ii> vii;
/******************************************************************************************/
   #ifdef DEBUG
        vector<string> split(const string &s, char c) {
            vector<string> v;
            stringstream ss(s);
            string x;
            while (getline(ss, x, c))
                v.emplace_back(x);
            return move(v);
        }

        template <typename T, typename... Args>
        inline string arrStr(T arr, int n) {
            stringstream s;
            s << "[";
            for (int i = 0; i < n - 1; i++)
                s << arr[i] << ",";
            s << arr[n - 1] << "]";
            return s.str();
        }

        #define debug(args...) \
        __line_no(__LINE__);   \
        __arg_val(split(#args, ',').begin(), args);

        inline void __line_no(int line){cerr<<"#"<<line<<": ";}
        template<typename T,typename... Args> 
        inline void __var_val(T *a){cerr<<arrStr(a,5);}
        template<typename T,typename... Args> 
        inline void __var_val(T a){cerr<<a;}
        inline void __arg_val(vector<string>::iterator it){cerr<<endl;}

        template<typename T,typename... Args>
        inline void __arg_val(vector<string>::iterator it,T a, Args... args) {
            cerr<<it->substr(0,it->length())<<" = ";
            __var_val(a);
            cerr<<" || ";
            __arg_val(++it,args...);
        }
    #else
        #define debug(args...)
    #endif
/****************************************************************************************************/
    #define deathnote()     ios_base::sync_with_stdio(0); cin.tie(0); cout.tie(0);
    #define endl            "\n"
    #define pb              push_back
    #define F               first
    #define S               second
    #define input(vec)      for(ll i=0;i<vec.size();cin>>vec[i++]);
    #define srt(vec)        sort(vec.begin(),vec.end());
    #define ln(s)           s.length()
    #define sz(vec)         vec.size()
    #define sp(n)           cout<<fixed<<setprecision(n);
    #define mem(x)          memset(x,0,sizeof(x));
    #define test()          ll t;cin>>t;while(t--)
    #define maxn            100005
    #define mod             (ll)(1e9+7)
    #define mod1            998244353  
/****************************************************************************************************/
    bool a[1005];
    ll power(ll x, ll y, ll p){
        ll res = 1;
        x = x % p;
        while (y > 0){
            if (y & 1) res = (res*x) % p;
            y = y>>1;
            x = (x*x) % p;
        }
        return res;
    }

    int main(){
        deathnote();
        FILE *f1 = fopen("2.txt","r");
        int i=0;
        fscanf(f1,"%d",&i);
        while(!feof(f1)){
        	a[i] = 1;
        	fscanf(f1,"%d",&i);
        }
        fclose(f1);

        f1 = fopen("1.txt","r");
		fscanf(f1,"%d",&i);
        while(!feof(f1)){
        	if(a[i]==0){
        		cout<<i<<endl;
        	}
        	fscanf(f1,"%d",&i);
        }
        fclose(f1);        

        #ifdef LOCAL
        cerr << "Time elapsed: " << 1.0 * clock() / CLOCKS_PER_SEC << " s.\n";
        #endif
        return 0;
    }
    