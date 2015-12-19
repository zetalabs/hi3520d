#!/bin/bash
valgrind --leak-check=full --show-reachable=yes -v ./check_xml 2>&1 | tee check_xml.log
valgrind --leak-check=full --show-reachable=yes -v ./check_json 2>&1 | tee check_json.log
valgrind --leak-check=full --show-reachable=yes -v ./check_upnp 2>&1 | tee check_upnp.log
valgrind --leak-check=full --show-reachable=yes -v ./check_ssi 2>&1 | tee check_ssi.log
