package main

import "github.com/ethereum/go-ethereum/crypto"
import "encoding/hex"
import "fmt"

func main() {
	
	//Create an account
	key, err := crypto.GenerateKey()
	if err != nil {
		fmt.Println("Error: ", err.Error());
	}
	
	//Get the address
	address := crypto.PubkeyToAddress(key.PublicKey).Hex()
	fmt.Printf("address[%d][%v]\n", len(address), address);
	
	//Get the private key
	privateKey := hex.EncodeToString(key.D.Bytes())
	fmt.Printf("privateKey[%d][%v]\n", len(privateKey), privateKey);
	
}
