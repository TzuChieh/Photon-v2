include(CheckCXXSourceRuns)

#--------------------------------------------------------------------------
# Check hardware features and record the result in cache variables. All
# checks will only run once unless the corresponding cache variable is
# removed.
#
#--------------------------------------------------------------------------
function(check_hardware_features)

    # List of Intel intrinsics: https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html

    # TODO: enable flags on other platforms using CMAKE_REQUIRED_FLAGS
    # TODO: depending on the detected hw features, set flags for compiler code gen
    # TODO: AVX512

    # Check SSE
    check_cxx_source_runs("
        #if defined(_MSC_VER)
            #include <immintrin.h>
        #else
            #include <x86intrin.h>
        #endif

        int main()
        {
            __m128 a, b;
            float data[4]{};
            a = _mm_loadu_ps(data);
            b = _mm_add_ps(a, a);
            _mm_storeu_ps(data, b);
            return 0;
        }"
        PH_HARDWARE_HAS_SSE)

    # Check SSE2
    check_cxx_source_runs("
        #if defined(_MSC_VER)
            #include <immintrin.h>
        #else
            #include <x86intrin.h>
        #endif

        int main()
        {
            __m128d a, b;
            double data[2]{};
            a = _mm_loadu_pd(data);
            b = _mm_add_pd(a, a);
            _mm_storeu_pd(data, b);
            return 0;
        }"
        PH_HARDWARE_HAS_SSE2)

    # Check SSE3
    check_cxx_source_runs("
        #if defined(_MSC_VER)
            #include <immintrin.h>
        #else
            #include <x86intrin.h>
        #endif

        int main()
        {
            __m128d a, b;
            double data[2]{};
            a = _mm_loadu_pd(data);
            b = _mm_hadd_pd(a, a);
            _mm_storeu_pd(data, b);
            return 0;
        }"
        PH_HARDWARE_HAS_SSE3)

    # Check SSSE3
    check_cxx_source_runs("
        #if defined(_MSC_VER)
            #include <immintrin.h>
        #else
            #include <x86intrin.h>
        #endif

        int main()
        {
            __m128i a, b;
            int data[4]{-1, -2, -3, -4};
            a = _mm_loadu_si128(reinterpret_cast<const __m128i*>(data));
            b = _mm_abs_epi32(a);
            _mm_storeu_si128(reinterpret_cast<__m128i*>(data), b);
            return 0;
        }"
        PH_HARDWARE_HAS_SSSE3)

    # Check SSE4.1
    check_cxx_source_runs("
        #if defined(_MSC_VER)
            #include <immintrin.h>
        #else
            #include <x86intrin.h>
        #endif

        int main()
        {
            __m128 a, b;
            float data[4]{1, 2, 3, 4};
            const int mask = 123;
            a = _mm_loadu_ps(data);
            b = _mm_dp_ps(a, a, mask);
            _mm_storeu_ps(data, b);
            return 0;
        }"
        PH_HARDWARE_HAS_SSE4_1)

    # Check SSE4.2
    check_cxx_source_runs("
        #if defined(_MSC_VER)
            #include <immintrin.h>
        #else
            #include <x86intrin.h>
        #endif

        int main()
        {
            long long dataA[2]{ 1, 2};
            long long dataB[2]{-1, 3};
            long long dataC[2]{};
            __m128i a = _mm_loadu_si128(reinterpret_cast<__m128i*>(dataA));
            __m128i b = _mm_loadu_si128(reinterpret_cast<__m128i*>(dataB));
            __m128i c = _mm_cmpgt_epi64(a, b);
            _mm_storeu_si128(reinterpret_cast<__m128i*>(dataC), c);
            return 0;
        }"
        PH_HARDWARE_HAS_SSE4_2)

    # Check AVX
    check_cxx_source_runs("
        #if defined(_MSC_VER)
            #include <immintrin.h>
        #else
            #include <x86intrin.h>
        #endif

        int main()
        {
            __m256 a, b;
            float data[8]{};
            a = _mm256_loadu_ps(data);
            b = _mm256_add_ps(a, a);
            _mm256_storeu_ps(data, b);
            return 0;
        }"
        PH_HARDWARE_HAS_AVX)

    # Check AVX2
    check_cxx_source_runs("
        #if defined(_MSC_VER)
            #include <immintrin.h>
        #else
            #include <x86intrin.h>
        #endif

        int main()
        {
            __m256i a, b;
            int data[8]{-1, -2, -3, -4, -5, -6, -7, -8};
            a = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(data));
            b = _mm256_abs_epi32(a);
            _mm256_storeu_si256(reinterpret_cast<__m256i*>(data), b);
            return 0;
        }"
        PH_HARDWARE_HAS_AVX2)

endfunction()
