#include "SensorManagement.h"

// --- CONFIGURATION PINOUT ---
// Définis ici pour ne pas polluer le reste du projet
#define BME_SCK  6
#define BME_MISO 5
#define BME_MOSI 4
#define BME_CS   3
#define VSPI  FSPI // Utilisation du bus FSPI pour l'ESP32-S3

#define I2C_SDA  41
#define I2C_SCL  42
#define TMG_ADDR 0x39

// --- CONSTRUCTEUR ---
SensorManager::SensorManager() 
    : _vspi(new SPIClass(VSPI)),        // Création de l'instance SPI dynamique
      _bme(BME_CS, _vspi),                 // Injection du SPI dans le driver BME
      _tmg(TMG_ADDR, &Wire1)               // Injection du Wire1 dans le driver TMG
{
}

// --- INITIALISATION ---
bool SensorManager::begin() {
    Serial.println("[SensorManager] Initializing Sensors...");
    bool success = true;

    // 1. Initialisation SPI & BME680
    _vspi->begin(BME_SCK, BME_MISO, BME_MOSI, BME_CS);
    pinMode(BME_CS, OUTPUT);
    digitalWrite(BME_CS, HIGH);

    if (!_bme.begin()) {
        Serial.println("[SensorManager] ERREUR: BME680 introuvable ! Vérifier câblage SPI.");
        success = false;
    } else {
        // Configuration BME pour l'économie d'énergie et la précision
        _bme.setTemperatureOversampling(BME680_OS_8X);
        _bme.setHumidityOversampling(BME680_OS_2X);
        _bme.setPressureOversampling(BME680_OS_4X);
        _bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
        // Désactivation du chauffage gaz pour économiser la batterie si non utilisé
        // _bme.setGasHeater(320, 150); 
        _bme.setGasHeater(0, 0); 
        Serial.println("[SensorManager] BME680 OK.");
    }

    // 2. Initialisation I2C & TMG3993
    Wire1.begin(I2C_SDA, I2C_SCL);
    
    if (!_tmg.initialize()) {
        Serial.println("[SensorManager] ERREUR: TMG3993 introuvable ! Vérifier câblage I2C.");
        success = false;
    } else {
        _tmg.setADCIntegrationTime(0xdb); // ~103ms
        _tmg.enableEngines(ENABLE_PON | ENABLE_AEN | ENABLE_AIEN);
        Serial.println("[SensorManager] TMG3993 OK.");
    }

    return success;
}

// --- LECTURE DES DONNÉES ---
void SensorManager::readAll(DataStruct& dataOut) {
    // --- Lecture BME680 ---
    if (_bme.performReading()) {
        dataOut.temperature = _bme.temperature;
        dataOut.humidity = _bme.humidity;
        dataOut.pressure = _bme.pressure / 100.0F; // Conversion Pa -> hPa
    } else {
        Serial.println("[SensorManager] Echec lecture BME !");
    }

    // --- Lecture TMG3993 ---
    // Vérification si les données sont valides (STATUS_AVALID)
    if (_tmg.getSTATUS() & STATUS_AVALID) {
        uint16_t r, g, b, c;
        _tmg.getRGBCRaw(&r, &g, &b, &c);
        
        // Calcul des Lux via la méthode de la librairie
        dataOut.light_intensity = (float)_tmg.getLux(r, g, b, c);
        
        // Nettoyage de l'interruption (important pour la prochaine lecture)
        _tmg.clearALSInterrupts();
    } else {
         Serial.println("[SensorManager] TMG Data not ready");
    }

    // Debug rapide
    Serial.printf("[Sensors] T: %.2f C | H: %.2f %% | P: %.2f hPa | Lux: %.2f\n", 
                  dataOut.temperature, dataOut.humidity, dataOut.pressure, dataOut.light_intensity);
}
