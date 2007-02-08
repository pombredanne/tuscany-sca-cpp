export AXIS2C_HOME=/home/slaws/apps/axis2c-bin-0.96-linux

export LIBXML2_LIB=/usr/lib
export LIBXML2_INCLUDE=/usr/include/libxml2

export PHP_LIB=/usr/local/lib
export PHP_INCLUDE=/usr/local/include/php

export PHP_SCA_SDO_INCLUDE=/home/slaws/phpbuild-5-2/pecl/SDO
export PHP_SCA_SDO_LIB=$PHP_LIB/php/extensions/no-debug-zts-20060613/

export TUSCANY_SDOCPP=/sdo/deploy
export TUSCANY_SCACPP=/usr/local/tuscany/cpp/sca/deploy

export LD_LIBRARY_PATH=$LIBXML2_LIB:\
$AXIS2C_HOME/lib:\
$PHP_LIB:\
$TUSCANY_SDOCPP/lib:\
$TUSCANY_SCACPP/lib:\
$TUSCANY_SCACPP/extensions/cpp/lib:\
$TUSCANY_SCACPP/extensions/php/lib:\
$TUSCANY_SCACPP/extensions/ws/lib:\
$TUSCANY_SCACPP/samples/PHPCalculator/deploy/sample.calculator:\
$PHP_SCA_SDO_LIB
                       
export TUSCANY_SCACPP_LOGGING=9
export TUSCANY_SCACPP_ROOT=/usr/local/tuscany/cpp/sca/deploy/samples/PHPCalculator

export PATH=.:$PATH