root4star<<EOF
TString n_events("$1")
int n = n_events.IsNull() ? 67 : n_events.Atoi()
.L genevents.C
genevents t(n)
t.Loop()
EOF
