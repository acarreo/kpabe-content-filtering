
curve="$1"

if [ "$curve" != "bls12-446" ]; then
    curve="bls12-381"
fi

cd ../
docker run -id --name benchmark --rm -v $PWD/:/root presto-content-filtering-use-case:relic-0.6.0--lsss

echo "Compiltation"
docker exec -ti benchmark make clean CURVE=$curve
docker exec -ti benchmark make CURVE=$curve

echo -e "\n-----------------------------------------------------------------------"
echo "Run all tests"
docker exec -ti benchmark make test CURVE=$curve | tee "benchmarks_$curve"

echo -e "\n-----------------------------------------------------------------------"
echo "Clean"
docker exec -ti benchmark make clean CURVE=$curve
docker kill benchmark
