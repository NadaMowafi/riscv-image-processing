
#include "BoxFilter_Vector_Notemp.hpp"


vector<vector<uint16_t>> BoxFilter(
    const vector<vector<uint16_t>> &inputImg,
    int kernelSize)
{
    if (inputImg.empty() || inputImg[0].empty()) {
        throw std::invalid_argument("Image is empty");
    }
    size_t rows = inputImg.size();
    size_t cols = inputImg[0].size();
    int border = kernelSize / 2;
    if (kernelSize > rows || kernelSize > cols || (kernelSize % 2) == 0) {
        throw std::invalid_argument("Invalid kernel size");
    }

    // Output, padded input, and temporary buffer
    vector<vector<uint16_t>> outputImg(rows, vector<uint16_t>(cols, 0));
    vector<vector<uint16_t>> padded = zeroPad(inputImg, kernelSize);
    vector<vector<uint16_t>> tempImg = padded;

    // --- Horizontal pass ---
    for (size_t i = 0; i < rows; ++i) {
        
        size_t j = 0;
        while (j < cols) {
            size_t vl = __riscv_vsetvl_e16m1(cols - j);
            // Prepare zero vector for accumulation
            vuint16m1_t zero16 = __riscv_vmv_v_x_u16m1((uint16_t)0, vl);
            vuint32m2_t vsum32 = __riscv_vmv_v_x_u32m2((uint32_t)0, vl);

            // Sum horizontally
            for (int k = -border; k <= border; ++k) {
                const uint16_t *ptr16 = &padded[i+border][j + border + k];
                vuint16m1_t v16 = __riscv_vle16_v_u16m1(ptr16, vl);
                // Widen-add: zero16 (0) + v16 → 32-bit
                vuint32m2_t v32 = __riscv_vwaddu_vv_u32m2(zero16, v16, vl);
                vsum32 = __riscv_vadd_vv_u32m2(vsum32, v32, vl);
                
            }

            // Compute average in 32-bit
            vuint32m2_t vavg32 = __riscv_vdivu_vx_u32m2(vsum32, kernelSize, vl);
            // Narrow back to 16-bit
            vuint16m1_t vavg16 = __riscv_vnclipu_wx_u16m1(vavg32, 0, 0, vl);
            // Store
            __riscv_vse16_v_u16m1(&tempImg[i+border][j + border], vavg16, vl);
            j += vl;
        }
    }

    // --- Vertical pass ---
    for (size_t i = 0; i < rows; ++i) {
          
        size_t j = 0;
        while (j < cols) {
            size_t vl = __riscv_vsetvl_e16m1(cols - j);
            vuint16m1_t zero16 = __riscv_vmv_v_x_u16m1((uint16_t)0, vl);
            vuint32m2_t vsum32 = __riscv_vmv_v_x_u32m2((uint32_t)0, vl);

            for (int k = -border; k <= border; ++k) {
                const uint16_t *ptr16 = &tempImg[i + border + k][j + border];
                vuint16m1_t v16 = __riscv_vle16_v_u16m1(ptr16, vl);
                vuint32m2_t v32 = __riscv_vwaddu_vv_u32m2(zero16, v16, vl);
                vsum32 = __riscv_vadd_vv_u32m2(vsum32, v32, vl);
            }

            vuint32m2_t vavg32 = __riscv_vdivu_vx_u32m2(vsum32, kernelSize, vl);
            vuint16m1_t vavg16 = __riscv_vnclipu_wx_u16m1(vavg32, 0, 0, vl);
            __riscv_vse16_v_u16m1(&outputImg[i][j], vavg16, vl);
            j += vl;
        }
    }

    return outputImg;
}

vector<vector<uint16_t>> zeroPad(
    const vector<vector<uint16_t>> &inputImg,
    int kernelSize)
{
    size_t rows = inputImg.size();
    size_t cols = inputImg[0].size();
    int border = kernelSize / 2;

    vector<vector<uint16_t>> padded(rows + 2 * border, vector<uint16_t>(cols + 2 * border, 0));

    for (int i = 0; i < rows; ++i) {
        int j = 0;
        while (j < cols) {
            size_t vl = __riscv_vsetvl_e16m1(cols - j);
            vuint16m1_t vec = __riscv_vle16_v_u16m1(&inputImg[i][j], vl);
            __riscv_vse16_v_u16m1(&padded[i + border][j + border], vec, vl);

            j += vl;
        }
    }

    return padded;
}

vector<vector<uint8_t>> BoxFilter(
    const vector<vector<uint8_t>> &inputImg,
    int kernelSize)
{
    if (inputImg.empty() || inputImg[0].empty()) {
        throw std::invalid_argument("Image is empty");
    }
    size_t rows = inputImg.size();
    size_t cols = inputImg[0].size();
    int border = kernelSize / 2;
    if (kernelSize > rows || kernelSize > cols || (kernelSize % 2) == 0) {
        throw std::invalid_argument("Invalid kernel size");
    }

    vector<vector<uint8_t>> outputImg(rows, vector<uint8_t>(cols, 0));
    vector<vector<uint8_t>> padded = zeroPad(inputImg, kernelSize);
    vector<vector<uint8_t>> tempImg = padded;

    // --- Horizontal pass ---
    for (size_t i = 0; i < rows; ++i) {
        size_t j = 0;
        while (j < cols) {
            size_t vl = __riscv_vsetvl_e8m1(cols - j);
            vuint8m1_t zero8 = __riscv_vmv_v_x_u8m1((uint8_t)0, vl);
            vuint16m2_t vsum16 = __riscv_vmv_v_x_u16m2((uint16_t)0, vl);

            for (int k = -border; k <= border; ++k) {
                const uint8_t *ptr8 = &padded[i + border][j + border + k];
                vuint8m1_t v8 = __riscv_vle8_v_u8m1(ptr8, vl);
                vuint16m2_t v16 = __riscv_vwaddu_vv_u16m2(zero8, v8, vl);
                vsum16 = __riscv_vadd_vv_u16m2(vsum16, v16, vl);
            }

            vuint16m2_t vavg16 = __riscv_vdivu_vx_u16m2(vsum16, kernelSize, vl);
            vuint8m1_t vavg8 = __riscv_vnclipu_wx_u8m1(vavg16, 0, 0, vl);
            __riscv_vse8_v_u8m1(&tempImg[i + border][j + border], vavg8, vl);
            j += vl;
        }
    }

    // --- Vertical pass ---
    for (size_t i = 0; i < rows; ++i) {

        size_t j = 0;
        while (j < cols) {
            size_t vl = __riscv_vsetvl_e8m1(cols - j);
            vuint8m1_t zero8 = __riscv_vmv_v_x_u8m1((uint8_t)0, vl);
            vuint16m2_t vsum16 = __riscv_vmv_v_x_u16m2((uint16_t)0, vl);

            for (int k = -border; k <= border; ++k) {
                const uint8_t *ptr8 = &tempImg[i + border + k][j + border];
                vuint8m1_t v8 = __riscv_vle8_v_u8m1(ptr8, vl);
                vuint16m2_t v16 = __riscv_vwaddu_vv_u16m2(zero8, v8, vl);
                vsum16 = __riscv_vadd_vv_u16m2(vsum16, v16, vl);
            }

            vuint16m2_t vavg16 = __riscv_vdivu_vx_u16m2(vsum16, kernelSize, vl);
            vuint8m1_t vavg8 = __riscv_vnclipu_wx_u8m1(vavg16, 0, 0, vl);
            __riscv_vse8_v_u8m1(&outputImg[i][j], vavg8, vl);
            j += vl;
        }
    }

    return outputImg;
}

vector<vector<uint8_t>> zeroPad(
    const vector<vector<uint8_t>> &inputImg,
    int kernelSize)
{
    size_t rows = inputImg.size();
    size_t cols = inputImg[0].size();
    int border = kernelSize / 2;

    vector<vector<uint8_t>> padded(rows + 2 * border, vector<uint8_t>(cols + 2 * border, 0));

    for (int i = 0; i < rows; ++i) {
        int j = 0;
        while (j < cols) {
            size_t vl = __riscv_vsetvl_e8m1(cols - j);
            vuint8m1_t vec = __riscv_vle8_v_u8m1(&inputImg[i][j], vl);
            __riscv_vse8_v_u8m1(&padded[i + border][j + border], vec, vl);
            j += vl;
        }
    }

    return padded;
}

vector<vector<uint32_t>> BoxFilter(
    const vector<vector<uint32_t>> &inputImg,
    int kernelSize)
{
    if (inputImg.empty() || inputImg[0].empty()) {
        throw std::invalid_argument("Image is empty");
    }
    size_t rows = inputImg.size();
    size_t cols = inputImg[0].size();
    int border = kernelSize / 2;
    if (kernelSize > rows || kernelSize > cols || (kernelSize % 2) == 0) {
        throw std::invalid_argument("Invalid kernel size");
    }

    vector<vector<uint32_t>> outputImg(rows, vector<uint32_t>(cols, 0));
    vector<vector<uint32_t>> padded = zeroPad(inputImg, kernelSize);
    vector<vector<uint32_t>> tempImg = padded;

    // --- Horizontal pass ---
    for (size_t i = 0; i < rows; ++i) {
        
        size_t j = 0;
        while (j < cols) {
            size_t vl = __riscv_vsetvl_e32m1(cols - j);
            vuint32m1_t vsum32 = __riscv_vmv_v_x_u32m1((uint32_t)0, vl);

            for (int k = -border; k <= border; ++k) {
                const uint32_t *ptr32 = &padded[ i + border][j + border + k];
                vuint32m1_t v32 = __riscv_vle32_v_u32m1(ptr32, vl);
                vsum32 = __riscv_vadd_vv_u32m1(vsum32, v32, vl);
            }

            vuint32m1_t vavg32 = __riscv_vdivu_vx_u32m1(vsum32, kernelSize, vl);
            __riscv_vse32_v_u32m1(&tempImg[ i + border][j + border], vavg32, vl);
            j += vl;
        }
    }

    // --- Vertical pass ---
    for (size_t i = 0; i < rows; ++i) {
        size_t j = 0;
        while (j < cols) {
            size_t vl = __riscv_vsetvl_e32m1(cols - j);
            vuint32m1_t vsum32 = __riscv_vmv_v_x_u32m1((uint32_t)0, vl);

            for (int k = -border; k <= border; ++k) {
                const uint32_t *ptr32 = &tempImg[i + border+ k][j + border];
                vuint32m1_t v32 = __riscv_vle32_v_u32m1(ptr32, vl);
                vsum32 = __riscv_vadd_vv_u32m1(vsum32, v32, vl);
            }

            vuint32m1_t vavg32 = __riscv_vdivu_vx_u32m1(vsum32, kernelSize, vl);
            __riscv_vse32_v_u32m1(&outputImg[i][j], vavg32, vl);
            j += vl;
        }
    }

    return outputImg;
}



// zeroPad for uint32_t
vector<vector<uint32_t>> zeroPad(
    const vector<vector<uint32_t>> &inputImg,
    int kernelSize)
{
    size_t rows = inputImg.size();
    size_t cols = inputImg[0].size();
    int border = kernelSize / 2;

    vector<vector<uint32_t>> padded(rows + 2 * border, vector<uint32_t>(cols + 2 * border, 0));

    for (int i = 0; i < rows; ++i) {
        int j = 0;
        while (j < cols) {
            size_t vl = __riscv_vsetvl_e32m1(cols - j);
            vuint32m1_t vec = __riscv_vle32_v_u32m1(&inputImg[i][j], vl);
            __riscv_vse32_v_u32m1(&padded[i + border][j + border], vec, vl);
            j += vl;
        }
    }

    return padded;
}