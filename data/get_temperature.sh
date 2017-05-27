#!/bin/sh -efu
FILENAME="air.sig995.2012.nc"
ID="972f08b9-e4b1-4edf-94e8-5ad7c4d8b33d"
URI="https://www.esrl.noaa.gov/psd/repository/entry/get/${FILENAME}?entryid=${ID}"
curl -L -o "${FILENAME}" "${URI}"
