#!/bin/sh -efu
FILENAME="ne_50m_admin_0_countries.zip"
HOST="www.naturalearthdata.com"
URI="http://${HOST}/http//${HOST}/download/50m/cultural/${FILENAME}"
curl -LO "${URI}"
unzip "${FILENAME}"
