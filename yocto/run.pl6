#!/star/u/djs232/local/bin/perl6
use v6;

sub MAIN(Str:D $n-files is copy, Str:D $which is copy, Str:D $AllTOF = "0", Str:D $TT_min = "4.0") {
    # Note: use "all" for n-files to get all the files
    my $catcmd;
    my $triggerid;
    if $which eq 's' {
        # $catcmd='catalog:star.bnl.gov?filetype=daq_reco_MuDst,events>0,production=P17id,filename~st_ssdmb,storage=local,year=2015,available=1';
        $triggerid = 500001;
    } elsif $which eq 't' {
        $which='t';
        # $catcmd = 'catalog:star.bnl.gov?filetype=daq_reco_MuDst,events>0,production=P17id,filename~st_ssdmb,storage=local,year=2015,available=1';
        $triggerid = 25;
    } else {
        $which='p';
        # $catcmd = 'catalog:star.bnl.gov?filetype=daq_reco_MuDst,events>0,production=P17id,filename~st_ssdmb,storage=local,year=2015,available=1';
        $triggerid = 500206;
    }


    unless $which.IO.e { mkdir $which; }
    for < input input/ScriptLocation input/ListLocation input/Location root out err > {
        unless "$which/$_".IO.e { mkdir "$which/$_"; }
    }
    # run "./clearall";
    #Get the pwd:
    my $cwd = ~$*CWD;
    my $Location       = "./$which/input/Location";
    my $ScriptLocation = "./$which/input/ScriptLocation";
    my $ListLocation =   "./$which/input/ListLocation";
    my $root = "$cwd/$which/root";
    my $out = "./$which/out";
    my $err = "./$which/err";

    # $catcmd.say;
    
    run "star-submit-template", "-template", "xml6_$which", "-entities", 
    "nfiles=$n-files,Location=$Location,ScriptLocation=$ScriptLocation,ListLocation=$ListLocation,root=$root,out=$out,err=$err,triggerid=$triggerid,TT_min=$TT_min,AllTOF=$AllTOF";
}
