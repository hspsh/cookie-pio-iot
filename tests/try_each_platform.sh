

if ! command -v pio &> /dev/null
then
    echo "Error - platformio executable not found."
    echo "Run this script from vscode pio context or install pio idc"
    exit
fi


TEST_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

#TUT - template under test
TUT_DIR=$TEST_DIR/..

echo Using template from: $TUT_DIR


TMP_DIR=/tmp/cookie-pio-iot

rm -rf $TMP_DIR

FILES=$TEST_DIR/*.yaml
for f in $FILES
do
  TEST_NAME=$(basename $f .yaml)
  echo "Processing $TEST_NAME.yaml"
  cookiecutter --config-file $f --no-input $TUT_DIR --output-dir $TMP_DIR
#   cat /tmp/$TEST_NAME/*/platformio.ini
  RES=$(\
    cd $TMP_DIR/$TEST_NAME/;\
    doppler setup -p hsp-iot-secrets -c dev;\
    ./generate_secret_header.py;\
    pio run -e proglocal;\
    )
  echo ""
done

# cookiecutter $TUT