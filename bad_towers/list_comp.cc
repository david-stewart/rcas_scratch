// good proof of concept code with which to update the get_bad_towers vector to read from a file

#include<vector>
#include<algorithm>
bool isIn(int val, std::vector<int> list){
    for (int i = 0; i < list.size(); ++i){
        if (val == list[i]) return true;
    }
    return false;
}

void list_comp() {
    vector<int> pp;
    vector<int> pAu;
    vector<int> both;
    vector<int> ppOnly;
    vector<int> pAuOnly;

    ifstream file;
    file.open("BadTowers_pAu200_2015.list");
    int val;
    while (file >> val) pp.push_back(val);
    file.close();
    file.open("BadTowersCombined_08.02.list");
    while (file >> val) pAu.push_back(val);
    file.close();

    /* std::sort(pAu.begin(), pAu.end()); */
    /* std::sort(pp.begin(), pp.end()); */

    for (int i = 0; i < pAu.size(); i++) {
        if (isIn(pAu[i], pp)) {
            both.push_back(pAu[i]);
        } else {
            pAuOnly.push_back(pAu[i]);
        }
    }
    for (int i = 0; i < pp.size(); i++) {
        if (!isIn(pp[i],both)) ppOnly.push_back(pp[i]);
    }

    FILE *f = fopen("list_comp","w");
    /* fprintf(f, "%i in pp  -> %i:both + %i:unique   || delta %i\n", */
    /*         pp.size(), both.size(), ppOnly.size(), pp.size() - both.size() - ppOnly.size()); */
    /* fprintf(f, "%i in pAu -> %i:both + %i:unique   || delta %i\n", */
    /*         pAu.size(), both.size(), pAuOnly.size(), pAu.size() - both.size() - pAuOnly.size()); */
    printf("%i in pp  -> %i:both + %i:unique   || delta %i\n",
            pp.size(), both.size(), ppOnly.size(), pp.size() - both.size() - ppOnly.size());
    printf("%i in pAu -> %i:both + %i:unique   || delta %i\n",
            pAu.size(), both.size(), pAuOnly.size(), pAu.size() - both.size() - pAuOnly.size());
    fclose(f);

    /* ofstream fout; */
    /* fout.open("BadTowersCombined_08.02.list"); */
    /* for (int i = 0; i < pp.size(); ++i){ */
    /*     fout << pp[i] << endl; */
    /* } */
    /* for (int i = 0; i < pAuOnly.size(); ++i){ */
    /*     fout << pAuOnly[i] << endl; */
    /* } */
    /* fout.close(); */

}
