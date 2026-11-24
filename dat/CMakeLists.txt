set(TTF_NAME "DejaVuSans.ttf")
set(TTF_URL "https://preesm.github.io/assets/downloads/${TTF_NAME}")

# Downloading font file
if(NOT EXISTS ${DAT_DIR}/${TTF_NAME})
	MESSAGE("Downloading ${TTF_NAME}")
	file(DOWNLOAD ${TTF_URL} "${DAT_DIR}/${TTF_NAME}"
			EXPECTED_MD5 BE189A7E2711CDF2A7F6275C60CBC7E2)
endif()

