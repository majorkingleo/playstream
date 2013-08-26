
# this way we can detect if it is a product and only in that case 
# it is required to install cpputils to the prod directory
if( $ENV{SITEDIR} || $ENV{EXTERNAL_CPPUTILS} ) {

&Rulemore('install', 'HEADERFILES', '',
        "\@mkdir -p \$($standard::ENVROOTDIR)/include && cp -f *.h \$($standard::ENVROOTDIR)/include");

&Rulemore('install', 'LIBS', '',
        "\@mkdir -p \$($standard::ENVROOTDIR)/lib && cp -f \$(LIBRARY) \$($standard::ENVROOTDIR)/lib");

}

$eof;
