#!/bin/bash
# test_irc.sh - Testes IRC (com cores)
# Uso: ./test_irc.sh [host] [port] [password]

HOST="${1:-127.0.0.1}"
PORT="${2:-6667}"
PASS="${3:-secret}"

TOTAL=0
PASSED=0

# Cores
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

pass() { echo -e "${GREEN}[PASS]${NC} $1"; PASSED=$((PASSED+1)); }
fail() { echo -e "${RED}[FAIL]${NC} $1"; }
info() { echo -e "${YELLOW}[INFO]${NC} $1"; }

# run_test <nome> <comandos> <expect_regex|"">
# Se expect="" → verifica que houve OUTPUT (não vazio) e sem timeout
run_test() {
    local name="$1"
    local cmds="$2"
    local expect="$3"
    local out

    TOTAL=$((TOTAL+1))

    out=$(printf "%b" "$cmds" | timeout 3 nc -q 1 "$HOST" "$PORT" 2>&1)
    local rc=$?

    # Detetar falha de rede/timeout
    if echo "$out" | grep -qi "Network is unreachable\|Connection timed out\|Connection refused"; then
        fail "$name (falha de rede: servidor não responde)"
        echo -e "${RED}--- output ---${NC}"
        echo "$out"
        echo -e "${RED}--------------${NC}"
        return
    fi
    if [ $rc -eq 124 ]; then
        fail "$name (timeout do comando)"
        return
    fi

    # Caso especial: não verificar output, só que não crashou
    if [ "$expect" = "SKIP_OUTPUT" ]; then
        pass "$name (sem crash)"
        return
    fi

    if [ -n "$expect" ]; then
        if echo "$out" | grep -Eq "$expect"; then
            pass "$name"
        else
            fail "$name (esperado: '$expect')"
            echo -e "${RED}--- output ---${NC}"
            echo "$out"
            echo -e "${RED}--------------${NC}"
        fi
    else
        if [ -z "$out" ]; then
            fail "$name (sem output — servidor não respondeu?)"
        else
            pass "$name (houve resposta, sem crash)"
        fi
    fi
}

echo -e "${BLUE}==========================================${NC}"
echo -e "${BLUE} Testes IRC contra ${HOST}:${PORT} (pass=${PASS})${NC}"
echo -e "${BLUE}==========================================${NC}"

# ------------------------------------------------------------------
# 1. PASS obrigatória
# ------------------------------------------------------------------
echo ""
echo -e "${BLUE}--- 1. PASS obrigatória ---${NC}"

run_test "Sem PASS → não regista (sem 001)" \
    "NICK alice\r\nUSER alice 0 * :Alice\r\n" \
    "464|ERROR|Pass Required"

run_test "PASS errada → 464" \
    "PASS wrongpass\r\nNICK alice\r\nUSER alice 0 * :Alice\r\n" \
    "464"

run_test "PASS correta + NICK + USER → 001" \
    "PASS $PASS\r\nNICK alice\r\nUSER alice 0 * :Alice\r\n" \
    "001 alice"

run_test "PASS depois de registado → 462" \
    "PASS $PASS\r\nNICK bob\r\nUSER bob 0 * :Bob\r\nPASS $PASS\r\n" \
    "462"

# ------------------------------------------------------------------
# 2. Registo
# ------------------------------------------------------------------
echo ""
echo -e "${BLUE}--- 2. Registo ---${NC}"

run_test "NICK sem parâmetro → 431" \
    "PASS $PASS\r\nNICK\r\n" \
    "431"

run_test "NICK duplicado → 433" \
    "PASS $PASS\r\nNICK carol\r\nUSER carol 0 * :Carol\r\nNICK alice\r\n" \
    "433"

#para registo falta testar comando USER quando argumentos invalidos,
#Se client tem nick e user, names configurados
#Verificar mensagem de boas vindas quando registo bem succedido
#pode adicionar outros testes que eu nao pensei

# ------------------------------------------------------------------
# 3. JOIN
# ------------------------------------------------------------------
echo ""
echo -e "${BLUE}--- 3. JOIN ---${NC}"

run_test "JOIN simples → JOIN broadcast" \
    "PASS $PASS\r\nNICK dave\r\nUSER dave 0 * :Dave\r\nJOIN #test\r\n" \
    "JOIN #test" 

run_test "JOIN canal inválido → 476" \
    "PASS $PASS\r\nNICK eve\r\nUSER eve 0 * :Eve\r\nJOIN test\r\n" \
    "476"

run_test "JOIN sem parâmetro → 461" \
    "PASS $PASS\r\nNICK f\r\nUSER f 0 * :F\r\nJOIN\r\n" \
    "461"

run_test "JOIN 0 → PART de todos" \
    "PASS $PASS\r\nNICK g\r\nUSER g 0 * :G\r\nJOIN #a\r\nJOIN #b\r\nJOIN 0\r\n" \
    "PART #a"

#para JOIN falta testar conteudo do broadcast para elem do nome do canal, como membros do canal, canal privado ou publico, end of list, etc.
#Tentar criar canal com nome invalido. Nome com espaco "#can al", sem #,&,!, etc.
#Criar ou se juntar em mais de um canal validos/ Criar ou se juntar em mais de um canal invalidos
#criar canal que ja exista
#Se juntar a canal que ja e membro
#verificar se membros sao notificados com entrada de novos membros
#pode adicionar outros testes que eu nao pensei

# ------------------------------------------------------------------
# 4. PRIVMSG
# ------------------------------------------------------------------
echo ""
echo -e "${BLUE}--- 4. PRIVMSG ---${NC}"

run_test "PRIVMSG nick inexistente → 401" \
    "PASS $PASS\r\nNICK h\r\nUSER h 0 * :H\r\nPRIVMSG ninguem :ola\r\n" \
    "401"

run_test "PRIVMSG sem texto → 412" \
    "PASS $PASS\r\nNICK i\r\nUSER i 0 * :I\r\nPRIVMSG alice\r\n" \
    "412"

run_test "PRIVMSG sem destinatário → 411" \
    "PASS $PASS\r\nNICK j\r\nUSER j 0 * :J\r\nPRIVMSG\r\n" \
    "411"

#falta testar: mensagem enviada para canal,  se todos os membros, recebem
#menssgem para canal em que nao e membro
#mensagem enviada para client, se mensagem chega
#mensagem para varios alvos, clientes e canais. Se IRC documentation permite
#mensagem mal formatada, sem :, espacos entre palavras nao sao intepretados, todo entre espaco e parametro
#pode adicionar outros testes que eu nao pensei

# ------------------------------------------------------------------
# 5. MODE
# ------------------------------------------------------------------
echo ""
echo -e "${BLUE}--- 5. MODE ---${NC}"

#para todos os modes testar se so operador consegue activar ou desativar,
#tudo verificando codigo retornado sempre que possivem, segundo a documentacao

run_test "MODE consulta canal → 324" \
    "PASS $PASS\r\nNICK k\r\nUSER k 0 * :K\r\nJOIN #m\r\nMODE #m\r\n" \
    "324"
#so operador, ver documentacao

run_test "MODE +i (operador) → broadcast" \
    "PASS $PASS\r\nNICK l\r\nUSER l 0 * :L\r\nJOIN #n\r\nMODE #n +i\r\n" \
    "MODE #n \\+i"
#falta testar: usar mode para ver modes do canal actualizado
#se user nao convidado pode juntar-se ao grupo,
#remover -i, e ver actualizacao,

run_test "MODE +k sem arg → sem crash" \
    "PASS $PASS\r\nNICK m\r\nUSER m 0 * :M\r\nJOIN #o\r\nMODE #o +k\r\n" \
    ""
#ver actualizacao dos modes
#ver se user com passe consegue entra, ver user sem passe e barrado
#Tentar definir passe novamente
#tentar remover passe -k com chave errada, remover com chave certa
#ver estado de modes
##pode adicionar outros testes que eu nao pensei

run_test "MODE modo inválido → 472" \
    "PASS $PASS\r\nNICK n\r\nUSER n 0 * :N\r\nJOIN #p\r\nMODE #p +z\r\n" \
    "472"

run_test "MODE por não-operador → 482" \
    "PASS $PASS\r\nNICK alber\r\nUSER ALBE 0 * :AL\r\nJOIN #p\r\nMODE #p +i\r\n" \
    "482"

#Falta testar: mode +l, adicionar limite de membros e ver memberos sao barrados apos limite
#remover limite -l, ver se clientes podem entrar,
#ver estado para ambos casos
#adicionar e remover varios modes ao mesmo tempo
#testar argumentos invalidos, e syntaxe errada
#Se grupo e notificado em qualquer operacao mode
#pode adicionar outros testes que eu nao pensei

# ------------------------------------------------------------------
# 6. KICK / INVITE / TOPIC
# ------------------------------------------------------------------
echo ""
echo -e "${BLUE}--- 6. KICK / INVITE / TOPIC ---${NC}"

run_test "KICK por não-operador → 482" \
    "PASS $PASS\r\nNICK do\r\nUSER DO 0 * :DO\r\nJOIN #p\r\nKICK #p alber\r\n" \
    "482"
#falta testar kick para user nao existente, para canal nao existente
#para canal em que client nao e membro, canal em que target nao e membro
#Se canal e notificado
#se target foi realmente removido do canal 
#pode adicionar outros testes que eu nao pensei

run_test "TOPIC consulta sem tópico → 331" \
    "PASS $PASS\r\nNICK q\r\nUSER q 0 * :Q\r\nJOIN #s\r\nTOPIC #s\r\n" \
    "331"

run_test "TOPIC set (operador) → broadcast" \
    "PASS $PASS\r\nNICK r\r\nUSER r 0 * :R\r\nJOIN #t\r\nTOPIC #t :novo topico\r\n" \
    "TOPIC #t"
#Testar Argumentos invalidos, canal nao existente, em que nao e membro, ou operador
#Testar mode +t e -t em conjuto com topic, usando o padrao de testes para mode ja acima
#ver se topic foi realmente alterado
#pode adicionar outros testes que eu nao pensei

run_test "INVITE nick inexistente → 401" \
    "PASS $PASS\r\nNICK s\r\nUSER s 0 * :S\r\nJOIN #u\r\nINVITE ninguem #u\r\n" \
    "401"
#Testar Argumentos invalidos, canal nao existente, em que nao e membro, ou operador
#convidar para canal privado e ver se convidado pode realmente entrar
#pode adicionar outros testes que eu nao pensei

# ------------------------------------------------------------------
# 7. PART
# ------------------------------------------------------------------
echo ""
echo -e "${BLUE}--- 7. PART ---${NC}"

run_test "PART canal onde não está → 442" \
    "PASS $PASS\r\nNICK t\r\nUSER t 0 * :T\r\nPART #p\r\n" \
    "442"

run_test "PART canal inexistente → 403" \
    "PASS $PASS\r\nNICK u\r\nUSER u 0 * :U\r\nPART #naoexiste\r\n" \
    "403"
#Testar Argumentos invalidos, como a falta de argumentos  ou alguns, validar comportamento razoavel
#Canal em que nao sou membro
#sair de um canal e ver se realmente ja nao sou membro, e se membros foram notificados
#pode adicionar outros testes que eu nao pensei


# ------------------------------------------------------------------
# 8. PING / PONG / CAP
# ------------------------------------------------------------------
echo ""
echo -e "${BLUE}--- 8. PING / PONG / CAP ---${NC}"

run_test "PING → PONG" \
    "PASS $PASS\r\nNICK v\r\nUSER v 0 * :V\r\nPING :token123\r\n" \
    "PONG.*token123"
#deixar como esta

run_test "CAP LS → lista vazia" \
    "CAP LS\r\n" \
    "CAP.*LS"
#deixar como esta

# ------------------------------------------------------------------
# 9. Robustez
# ------------------------------------------------------------------
echo ""
echo -e "${BLUE}--- 9. Robustez ---${NC}"

run_test "Comando desconhecido → 421" \
    "PASS $PASS\r\nNICK w\r\nUSER w 0 * :W\r\nFOOBAR\r\n" \
    "421"

run_test "Linhas vazias → sem crash" \
    "\r\n\r\n\r\n" \
    "SKIP_OUTPUT"

run_test "Espaços múltiplos → sem crash" \
    "PASS $PASS\r\nNICK x\r\nUSER x 0 * :X\r\nJOIN    #w\r\n" \
    "JOIN #w"

run_test "Linha muito longa → sem crash" \
    "PASS $PASS\r\nNICK y\r\nUSER y 0 * :Y\r\n$(printf 'A%.0s' {1..5000})\r\n" \
    ""

run_test "Comando com \r\n extra → sem crash" \
    "PASS $PASS\r\nNICK z\r\nUSER z 0 * :Z\r\nJOIN #x\r\n\r\n\r\n" \
    ""
run_test "Nick com caracteres inválidos → 432" \
    "PASS $PASS\r\nNICK 'a b'\r\n" \
    "432"
#pode adicionar outros testes que eu nao pensei em robustez e manter os actuais

# ------------------------------------------------------------------
# 10. Múltiplos clientes (manual)
# ------------------------------------------------------------------
echo ""
echo -e "${BLUE}--- 10. Múltiplos clientes (manual) ---${NC}"
info "Para testar broadcast, abre dois terminais:"
info "  Terminal 1: nc -C $HOST $PORT"
info "  Terminal 2: nc -C $HOST $PORT"
info "  T1: PASS $PASS / NICK a / USER a 0 * :A / JOIN #z"
info "  T2: PASS $PASS / NICK b / USER b 0 * :B / JOIN #z"
info "  T1 deve ver: :b!b@... JOIN #z"

#testar multiplos canais/ multiplos clientes com o objectivo que causar crash

# ------------------------------------------------------------------
# Contagem final
# ------------------------------------------------------------------
echo ""
echo -e "${BLUE}==========================================${NC}"
if [ "$PASSED" -eq "$TOTAL" ]; then
    echo -e "${GREEN} RESULTADO: $PASSED / $TOTAL testes passaram ✅${NC}"
    echo -e "${BLUE}==========================================${NC}"
    exit 0
else
    echo -e "${RED} RESULTADO: $PASSED / $TOTAL testes passaram ❌${NC}"
    echo -e "${BLUE}==========================================${NC}"
    exit 1
fi

#actualizar contagem de testes.
#pode adicionar novas categorias de testes que eu nao pensei, mas mantendo os testes actuais