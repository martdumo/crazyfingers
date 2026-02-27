/**
 * Easter Egg Generator for CrazyFingers
 * ES6 Module - Generates absurd facts about guitar playing
 */

// ============================================================================
// Array 1: Subjects (50 elements)
// ============================================================================

const SUBJECTS = [
    "George Bush",
    "Albert Einstein",
    "el Papa Francisco",
    "un alienigena de Andromeda",
    "Darth Vader",
    "tu profesor de matematicas",
    "un pinguino emperador",
    "Chuck Norris",
    "Batman",
    "un velociraptor",
    "el fantasma de Beethoven",
    "un ninja cibernetico",
    "Elon Musk",
    "un oso panda",
    "Julio Cesar",
    "el Yeti",
    "un samurai jubilado",
    "una inteligencia artificial",
    "el monstruo del Lago Ness",
    "un pirata del Caribe",
    "Gandalf",
    "un canguro boxeador",
    "Dracula",
    "un vikingo vegano",
    "Spiderman",
    "Napoleon Bonaparte",
    "un agente secreto",
    "el mismisimo diablo",
    "un mimo frances",
    "un chaman urbano",
    "tu ex pareja",
    "un politico corrupto",
    "un influencer de Instagram",
    "un banquero de Wall Street",
    "un vendedor de enciclopedias",
    "un fantasma en pena",
    "un zombie con depresion",
    "un vampiro vegetariano",
    "un hombre lobo con alergias",
    "un esqueleto en el armario",
    "un payaso asesino",
    "un nino poseido",
    "la Parca con resaca",
    "un demonio en paro",
    "un angel caido del cielo",
    "un extraterrestre abducido",
    "un robot con ansiedad",
    "un cyborg desactualizado",
    "un dios menor olvidado",
    "el autor de este codigo"
];

// ============================================================================
// Array 2: Actions (50 elements) - Dark humor included
// ============================================================================

const ACTIONS = [
    "aprendio a tocar la guitarra",
    "vendio su alma por un pedal de distorsion",
    "memorizo todas las escalas exoticas",
    "rompio tres cuerdas en un solo bend",
    "compuso un solo de 40 minutos",
    "domino el sweep picking",
    "afino su guitarra usando telequinesis",
    "construyo un amplificador con chatarra",
    "toco un concierto entero con los dientes",
    "se tatuo una tablatura en la espalda",
    "lloro al escuchar un acorde menor",
    "invento una pua indestructible",
    "durmio abrazado a una Fender Stratocaster",
    "uso un arco de violin en su guitarra",
    "desafino a proposito para sonar mas jazz",
    "toco la guitarra en gravedad cero",
    "se peleo con el baterista",
    "rompio su instrumento contra el suelo",
    "hizo un pacto con un demonio blusero",
    "descubrio el acorde prohibido",
    "grabo un disco en el bano",
    "aprendio a tocar con los pies",
    "hizo tapping a la velocidad de la luz",
    "fundo una banda de death metal",
    "toco un arpegio imposible",
    "uso cuerdas de alambre de puas",
    "se quedo sordo por culpa del fuzz",
    "robo la pua del destino",
    "hipnotizo al publico con un vibrato",
    "toco un tritono invocando el caos",
    "quemo su guitarra en un ritual",
    "sacrifico su gato para afinar mejor",
    "cambio su rinon por una Gibson Les Paul",
    "toco hasta que le sangraron los dedos",
    "uso sus propias venas como cuerdas",
    "grabó un solo en un cementerio",
    "invoco a Cthulhu con un riff",
    "se corto las venas con las cuerdas",
    "toco un solo postumo desde el mas alla",
    "vendio a su madre por un Marshall",
    "uso huesos humanos como plectros",
    "toco mientras el mundo se incendiaba",
    "sacrifico su carrera por un buen tono",
    "se emborracho con whisky y distorsion",
    "toco naked en publico",
    "uso electricidad real en los cables",
    "se electrocuto por un solo epico",
    "toco hasta que sus manos se carbonizaron",
    "firmo un contrato con sangre",
    "matio a alguien con una guitarra"
];

// ============================================================================
// Array 3: Reasons (50 elements) - Dark humor included
// ============================================================================

const REASONS = [
    "para lidiar mejor con su suegra",
    "porque se lo recomendo su terapeuta",
    "para impresionar a una chica en un bar",
    "mientras escapaba de la policia",
    "para pagar sus deudas de juego",
    "despues de comerse una pizza vencida",
    "porque perdio una apuesta",
    "en medio de un apocalipsis zombie",
    "para comunicarse con los delfines",
    "mientras viajaba en el tiempo",
    "para dominar el mundo",
    "porque el WiFi se habia caido",
    "en un ritual para atraer la lluvia",
    "para vengarse de su ex",
    "mientras caia en paracaidas",
    "para curar su insomnio",
    "porque confundio la guitarra con un hacha",
    "en un intento de llamar la atencion",
    "para ganar un concurso de talentos en Marte",
    "despues de ser abducido por extraterrestres",
    "para espantar a los osos",
    "porque queria ser el nuevo Jimi Hendrix",
    "en medio de un ataque de panico",
    "para conseguir seguidores en TikTok",
    "porque creyo que era un instrumento magico",
    "mientras cocinaba fideos",
    "para evitar lavar los platos",
    "en una crisis de mediana edad",
    "porque se lo dictaron los espiritus",
    "para detener una invasion alienigena",
    "porque su perro se lo pidio",
    "en un momento de debilidad mental",
    "para olvidar un trauma infantil",
    "porque estaba muy aburrido",
    "en un intento de suicidio fallido",
    "para expiar sus pecados",
    "porque perdio el trabajo",
    "en venganza contra la humanidad",
    "para financiar su adiccion",
    "porque le dijeron que era imposible",
    "en un arrebato de locura temporal",
    "para impresionar a su psiquiatra",
    "porque le debia dinero a la mafia",
    "en un culto satanico de barrio",
    "para revivir a su mascota muerta",
    "porque le iba mal en la vida",
    "en un episodio de esquizofrenia",
    "para evitar la carcel",
    "porque nadie lo invitaba a fiestas",
    "en sus ultimos minutos de vida"
];

// ============================================================================
// Functions
// ============================================================================

/**
 * Generate a random absurd fact
 * @returns {string} Absurd fact string
 */
export function generateAbsurdFact() {
    const subjectIndex = Math.floor(Math.random() * SUBJECTS.length);
    const actionIndex = Math.floor(Math.random() * ACTIONS.length);
    const reasonIndex = Math.floor(Math.random() * REASONS.length);

    return `Sabia usted que ${SUBJECTS[subjectIndex]} ${ACTIONS[actionIndex]} ${REASONS[reasonIndex]}?`;
}
