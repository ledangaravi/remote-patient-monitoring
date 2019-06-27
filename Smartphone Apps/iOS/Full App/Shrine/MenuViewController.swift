//
//  MenuViewController.swift
//  Shrine
//
//  Created by Zaid Aggad on 06/06/2019.
//  Copyright © 2019 Google. All rights reserved.
//

import UIKit
import MaterialComponents
let alerts: alerHistoryR  = UIStoryboard(name: "Main", bundle: nil).instantiateViewController(withIdentifier: "alert") as! alerHistoryR
class MenuViewController: UIViewController {
    var appBarViewController = MDCAppBarViewController()
     var delegate:SendManager?
    override func viewDidLoad() {
        super.viewDidLoad()
        super.viewDidLoad()
        self.view.backgroundColor = .white
        self.appBarViewController.navigationController?.navigationBar.backgroundColor = GMColor.imperialblue()
        self.appBarViewController.headerView.backgroundColor = GMColor.imperialblue()
        self.appBarViewController.navigationController?.navigationBar.backgroundColor = GMColor.imperialblue()
        self.view.tintColor = .white
        let titleDict: NSDictionary = [NSAttributedStringKey.foregroundColor: UIColor.white]
        self.appBarViewController.navigationBar.titleTextAttributes = (titleDict as! [NSAttributedStringKey : Any])
        self.title = "Menu"
        self.addChildViewController(self.appBarViewController)
        self.view.addSubview(self.appBarViewController.view)
        self.appBarViewController.didMove(toParentViewController: self)
        
        let menuItemImage = UIImage(named: "MenuItem")
        let templatedMenuItemImage = menuItemImage?.withRenderingMode(.alwaysTemplate)
        let menuItem = UIBarButtonItem(image: templatedMenuItemImage,
                                       style: .plain,
                                       target: self,
                                       action: nil)
        self.navigationItem.leftBarButtonItem = menuItem

        // Do any additional setup after loading the view.
    }
    
    
    @IBAction func dashboard(_ sender: Any) {
        print("dashboard")
      //  self.dismiss(animated: true, completion: nil)
        //let home:HomeViewController = UIStoryboard(name: "Main", bundle: nil).instantiateViewController(withIdentifier: "home") as! HomeViewController
        self.dismiss(animated: true, completion: nil)

    }
    
    
    @IBAction func settings(_ sender: Any) {
         let settings:settingsPage = UIStoryboard(name: "Main", bundle: nil).instantiateViewController(withIdentifier: "test") as! settingsPage
         let transition = CATransition()
         transition.duration = 0.3
         transition.type = kCATransitionPush
         transition.subtype = kCATransitionFromRight
         transition.timingFunction = CAMediaTimingFunction(name: kCAMediaTimingFunctionEaseInEaseOut)
         view.window!.layer.add(transition, forKey: kCATransition)
         self.present(settings, animated: false, completion: nil)
    }
    
    @IBAction func aboutb(_ sender: Any) {
        let about: about  = UIStoryboard(name: "Main", bundle: nil).instantiateViewController(withIdentifier: "about") as! about
        let transition = CATransition()
        transition.duration = 0.3
        transition.type = kCATransitionPush
        transition.subtype = kCATransitionFromRight
        transition.timingFunction = CAMediaTimingFunction(name: kCAMediaTimingFunctionEaseInEaseOut)
        view.window!.layer.add(transition, forKey: kCATransition)
        self.present(about, animated: false, completion: nil)
    }
   
     @IBAction func quietb(_ sender: Any) {
        let quiet: quietMode  = UIStoryboard(name: "Main", bundle: nil).instantiateViewController(withIdentifier: "quiet") as! quietMode
        let transition = CATransition()
        transition.duration = 0.3
        transition.type = kCATransitionPush
        transition.subtype = kCATransitionFromRight
        transition.timingFunction = CAMediaTimingFunction(name: kCAMediaTimingFunctionEaseInEaseOut)
        view.window!.layer.add(transition, forKey: kCATransition)
        self.present(quiet, animated: false, completion: nil)
     }
    
    @IBAction func alertb(_ sender: Any) {
        let transition = CATransition()
        transition.duration = 0.3
        transition.type = kCATransitionPush
        transition.subtype = kCATransitionFromRight
        transition.timingFunction = CAMediaTimingFunction(name: kCAMediaTimingFunctionEaseInEaseOut)
        view.window!.layer.add(transition, forKey: kCATransition)
        self.present(alerts, animated: false, completion: nil)
    }
    
    
     /*
     // MARK: - Navigation

    // In a storyboard-based application, you will often want to do a little preparation before navigation
    override func prepare(for segue: UIStoryboardSegue, sender: Any?) {
        // Get the new view controller using segue.destination.
        // Pass the selected object to the new view controller.
    }
    */

}
